# OpenAIRAC Architecture Specification (v0.4 AIRAC Lifecycle, Reconciliation & Distribution)

## Overview

OpenAIRAC is an open navigation-data engine designed to provide continuously maintainable navigation data for flight simulation without relying on rigid manual monthly AIRAC cycle overwrites.

## Dependency Direction & Core Boundaries

```text
[Data Providers (OurAirports / FAA CIFP)]
                    │
                    ▼
[Canonical Navigation Model (openairac-model)]
                    │
                    ▼
[Temporal Database Store (openairac-store)]
                    │
                    ├──► [Geomagnetic Physics Engine (openairac-magnetic)]
                    ├──► [Airway Graph + Geodesics (openairac-routing)]
                    └──► [Procedure Semantics (openairac-procedures)]
                              │
                              ▼
                    [Flight-Plan Integration (openairac-integration)]
                              │
                              ▼
                    [WorldQuery Service (openairac-service)]
                    │
                    ▼
[Exporters / CLI / Plugins (openairac-export-xplane, openairac-cli, openairac-plugin)]
```

## Crate Layout & Responsibilities

1. **`openairac-model`**: Strongly typed domain entities (`CanonicalAirport`, `CanonicalRunway`, `CanonicalNavaid`, `CanonicalWaypoint`, `CanonicalAirwayLeg`, `CanonicalProcedureLeg`), strongly-typed IDs (`AirportId`, `RunwayId`, `NavaidId`, `WaypointId`, `AirwayLegId`, `ProcedureLegId`, `SourceSnapshotId`, `WorldRevisionId`), frequency units (`FrequencyKhz`), and temporal source provenance (`SourceSnapshot` incl. `license_id`, `TemporalValidity`).
2. **`openairac-store`**: Embedded SQLite database (foreign keys + WAL) with versioned migrations (`PRAGMA user_version`, currently v8: procedure_legs, terminal waypoints, AIRAC lifecycle (v5), publications/tombstones (v6), reconciliation (v7), conflict dedup (v8)). Entity tables are keyed by `(id, valid_from)`: every ingestion appends a new temporal revision and closes the previous one (`valid_until` exclusive), so previous/current/future worlds are simultaneously queryable via `query_*_at(timestamp)`. **Payload revision is separated from provenance**: the payload comparison excludes the source snapshot id, so a new dataset snapshot does not re-revise unchanged entities; every accepted observation is logged in `entity_observations` instead. Preloaded future revisions can be corrected (same `valid_from`, replaced before becoming effective); effective history is immutable. Writers live at connection level so a whole ingestion runs in one transaction (`WorldStore::transact`). `validate()` performs structural integrity checks: provenance FK, runway→airport references, coordinate/temporal ranges, frequency bands, overlapping revisions, altitude descriptor/band consistency, ARINC 424 terminator membership, duplicate procedure sequences, procedure-fix and airway-endpoint references.
3. **`openairac-magnetic`**: Pure-Rust implementation of the official NOAA/NCEI World Magnetic Model 2025 (degree N=12). Golden-tested against all 12 official NOAA WMM2025 test vectors (0.3 nT / 0.02°). Runway magnetic drift analysis is strictly separated: published designators are never overwritten by WMM predictions.
4. **`openairac-ingest`**: `DataProvider` trait (fetch + transactional parse/ingest), deterministic `IngestReport` diagnostics (seen/parsed/created/updated/unchanged/quarantined/rejected/warnings/errors/duration/checksum). OurAirports CSV importer (live HTTP fetch, fail-closed validation). FAA CIFP adapter with a layered pipeline: fixed-width decoding → raw ARINC 424 records → semantic interpretation → canonical entities. Supported record classes (cycle 2608 verified): `EA` enroute waypoints, `D`/`DB`/`PN` navaids, `ER` airways, `PA` terminal airports, `PG` terminal runways, `PC` terminal waypoints, `PD`/`PE`/`PF` SID/STAR/approach legs. Unsupported record types are explicit and their raw lines preserved (`raw` column).
5. **`openairac-procedures`**: Semantic layer for instrument procedures. `PathTerminator` covers the ARINC 424 set (IF/TF/CF/DF/FA/FC/FD/FM/CA/CD/CI/CR/VA/VD/VI/VM/VR/HA/HF/HM/RF); unknown terminators are preserved via `Unsupported(String)` with diagnostics — never guessed. `ProcedureLeg::interpret` maps each terminator's positional fields to semantics (course/heading, altitude bands normalized from `B` bands, RF radius+turn, recommended-navaid references). `Procedure::assemble` groups legs into main body + transitions, orders by sequence, rejects mixed temporal instants; `validate()` reports sequence gaps/duplicates and missing fix coordinates. Geometric rendering (e.g. RF arc shapes) is deliberately out of scope — constraints only.
6. **`openairac-routing`**: Geodesic WGS84 distance/bearing (`DirectRoute::between`) with validated `Coordinate`; the canonical `AirwayGraph` (NodeId = ident + ICAO region + entity class; edges from airway legs; endpoints resolve as fixes or navaids). Dijkstra/A* with geodesic heuristic; `RouteRequest` supports cruise-altitude filtering against MEA/max-altitude bands, RNAV gating, node/route exclusions; `RouteResult` carries legs and diagnostics. `disconnected_components()` supports data-quality reporting. Temporal validity is a construction invariant: only entities valid at the build instant can enter the graph.
7. **`openairac-integration`**: Flight-plan planner reading only the temporal store. Assembles SID/STAR/approach via the semantic layer (procedure identity and transitions preserved), joins the airway graph at SID exit fix → STAR entry fix using published waypoint region codes (never guessed), and fails closed when either end lacks a procedure or join-fix regions are unknown.
8. **`openairac-service`**: UI-independent query boundary (`WorldQuery`): `world_at` counts, airport lookup/search, coordinate proximity (airports + navaids), airway summaries, procedure summaries, graph component sizes, and full flight planning. Synchronous JSON-serializable DTOs; no SQLite types leak.
9. **`openairac-export-xplane`**: X-Plane 12 exporter implementing Laminar's XPFIX1200 / XPNAV1200 / XPAWY1101 specifications; row conventions are cross-checked against Laminar convert424toxplane v12.4 output on the same FAA CIFP input. Emits `earth_fix.dat` (VOR-DME/VORTAC/ILS facilities include their paired DME rows), `earth_nav.dat`, `earth_awy.dat` (endpoint typing 11/2/3 with referential integrity against the actually-exported fix/nav entities, airway-name merging) and a checksummed `manifest.json`. Fail-closed: rows missing source-provided values are skipped with diagnostics (stricter than convert424toxplane, which defaults unknown elevations to 0); files are STAGED and then swapped file-by-file — the multi-file swap is not atomic as a set (a crash mid-swap can leave a mixed layer); incomplete layers are refused without `--allow-empty`. This is NOT a globally installable layer yet and the CLI does not install into live simulators — the transactional backup/rollback install is designed (`InstallPlan`, manifest) but intentionally not implemented or exposed.
   - **Pipeline decision (investigated)**: Laminar's convert424toxplane v12.4 was downloaded and run against the real FAA CIFP cycle 2608 — it produces the complete XP12 file set and is the gold standard for conversion. The long-term preferred final pipeline is a full worldwide ARINC 424 master file fed to convert424toxplane (or shipped as `earth_424.dat`); the native exporter remains the canonical-store path and its fixtures are validated against the tool's output. Our exporter deliberately diverges from the tool where the tool fabricates (NDB elevation 0, synthesized glideslope geometry, blank-level airway defaults): we skip and diagnose instead. See `docs/X_PLANE_STRATEGY.md`.
10. **`openairac-plugin`**: X-Plane 12 C-ABI plugin (`OpenAIRAC_QueryWorldStatus`) exposing local SQLite status to the simulator without mutation.
11. **`openairac-reconcile`**: deterministic multi-source reconciliation over the temporal store — canonical identities (natural-key-derived, never provider ids), source memberships with exact revision intervals, structured evidence and conflicts, authority policy, resolved read-side view.
12. **`openairac-bundle`**: deterministic content-addressed distribution bundles (manifest core hash, per-file integrity, explicit UnsignedDevelopment authenticity), staged validated install, artifact-level rollback, local update channel with deterministic decisions (NoUpdate/Preload/Activate/ReplacePreload/RejectIncompatible/RejectInvalid).
13. **`openairac-cli`**: `doctor`, `magnetic`/`magvar`, `magdrift`, `sync`, `status`, `validate`, `export xplane`, `cycle discover/list/observe/rollback`, `reconcile`, `bundle build/inspect/verify/install/list/rollback`, `update check/apply`.
