# OpenAIRAC Product Roadmap

## Milestone v0.2 — Foundation Reboot (Shipped)
- [x] NOAA WMM2025 spherical harmonic solver, golden-tested against all 12 official NOAA test vectors
- [x] Runway magnetic drift detector & wrap analyzer (published data never overwritten)
- [x] Canonical domain model with strongly-typed IDs & provenance (`license_id`)
- [x] Temporal SQLite store with real revisioning (`(id, valid_from)`), preloading, and versioned migrations
- [x] Transactional, fail-closed ingestion with deterministic diagnostics (reject/quarantine/unchanged)
- [x] Live OurAirports ingestion (HTTP fetch) + offline fixtures
- [x] FAA CIFP layered decoder (EA/D/DB/PN/ER; explicit unsupported records; real-cycle fixtures)
- [x] Structural store validation (`openairac validate`, `WorldStore::validate`)
- [x] Geodesic WGS84 great-circle distance direct routing
- [x] X-Plane 12 dat exporter per Laminar XPFIX1200/XPNAV1200/XPAWY1101 specs (staged sequential swap, fail-closed ILS, golden fixtures)
- [x] X-Plane 12 C-API plugin for local DB status querying
- [x] CLI commands (`doctor`, `magnetic`/`magvar`, `magdrift`, `sync`, `status`, `validate`, `export xplane`)
- [x] CI workflows and automated workspace validation (fmt/check/clippy -D warnings/tests)

## Milestone v0.3 — Routing & Procedures Foundation (Shipped)
- [x] Canonical airway routing graph (Dijkstra/A\*) with temporal validity, direction semantics, MEA/cruise-altitude filtering, RNAV gating, and exclusions
- [x] FAA CIFP terminal record classes: PA/PG/PC terminal waypoints and PD/PE/PF procedure legs (cycle 2608 verified, lossless raw preservation)
- [x] ARINC 424 SID / STAR / Approach semantic layer with typed path-terminator interpretation (fail-closed on unsupported semantics)
- [x] Flight-plan integration: airport → SID → enroute → STAR → approach with procedure identity and transitions preserved
- [x] Data-quality validation: endpoint existence, procedure fix references, sequence duplicates, altitude bands, terminator membership, disconnected components
- [x] WorldQuery service boundary (world_at / search / nearby / airways / procedures / plan)
- [x] X-Plane production-path strategy documented (convert424toxplane for earth_424.dat; native exporter = diagnostics)
- [x] X-Plane 12 `earth_awy.dat` native exporter (XPAWY1101, endpoint typing 11/2/3, referential integrity)

## Milestone v0.4 — AIRAC Lifecycle, Reconciliation & Distribution (Shipped)

The engine is now self-updating, provenance-complete, and
distributable as verified artifacts.

- [x] AIRAC cycle catalog with a validated state machine (Discovered → Preloaded → Active → Superseded/Expired/RolledBack)
- [x] Cycle discovery (FAA CIFP directory listing; effective dates UNCONFIRMED until confirmed — fail-closed)
- [x] Explicit `CycleSelector` (cycle ident / source URI / confirmed effective date — never wall-clock-inferred)
- [x] Preload with `Scheduled` events and atomic transactional `observe_cycles`
- [x] Full-snapshot removal semantics (`close_absent_at` with namespace-scoped, masked, temp-seen-set application)
- [x] Differential publications (absence means nothing) and first-class tombstones
- [x] Corrections: future-revision replacement, withdrawal, post-effective new revisions; nanosecond boundary semantics
- [x] Publication identity: replay idempotent, conflicting content fails loudly unless a Correction
- [x] One atomic transaction per publication: snapshot + identity guard + payloads + tombstones + close + audit + lifecycle bookkeeping
- [x] Rollback by re-publication (history immutable, provider-scoped diff, exact provenance)
- [x] Multi-source entity reconciliation (canonical identities, memberships with exact source intervals, evidence, conflicts, authority policy, resolved view)
- [x] Deterministic content-addressed data bundles (manifest + integrity + UnsignedDevelopment authenticity)
- [x] Staged, validated bundle install; artifact-level rollback; local update channel with deterministic decisions
- [x] Real-data validated: FAA CIFP 2608 (396,460 records) + live OurAirports (85,912 airports / 14,393 navaids) reconciled to 1,431 exact canonical matches

## Milestone v0.5 — Procedure Fidelity & Geometry (Shipped)

- [x] FAA PA/PG terminal airports and runways decoded (13,316 airports, 8,149 runway pairs from cycle 2608; verified DMS coordinates, tens-of-feet lengths, reciprocal-end pairing)
- [x] ILS localizer/glideslope/runway associations from PF records (1,175 associations, 715 localizers enriched with verified bearing/glideslope; category never fabricated)
- [x] Hold semantics verified (fix-based HA/HF/HM, inbound course from course_b)
- [x] RF arc geometry (geodesic center derivation, turn-direction arcs, circle-verified sampling, fail-closed)
- [x] Procedure completeness diagnostics (deterministic severities: unresolved fixes, unsupported terminators, impossible bands, missing RF data, runway mismatches)
- [ ] Remaining verified ARINC path-terminator semantics (vertical-angle GP legs, course-C DF legs, MSA sectors)

## Milestone v0.6 — Worldwide Coverage & Providers (Shipped)

- [x] Worldwide provider architecture: registry-driven provider
  selection (`provider_constructors()`), unified publication path
  (OurAirports and FAA CIFP both apply atomically through the same
  dataset-publication machinery — identity guard, tombstones,
  close_absent, audit)
- [x] Provider capabilities in manifests (coverage scope, temporal
  model, update model, authority note) + `coverage` report with
  per-provider entity counts, snapshots, ILS associations, and
  reconciliation conflicts
- [x] Data-driven regional authority: region-scoped authority rules
  (US: FAA first; world default: open metadata first), exposed via the
  resolved view — replaces static US-only assumptions
- [x] Worldwide source licensing research (DATA_SOURCES.md): only
  legally redistributable sources ship in bundles; proprietary sets
  rejected; ODbL/AIP candidates need per-license review
- [ ] Regional coverage expansion beyond US CIFP (candidates vetted
  legally; none implemented yet — needs per-country license review)
- [ ] Provider failover (designed: never mix worlds; not yet exercised)

## Milestone v0.7 — Simulator Output & Distribution Maturity (Shipped)

- [x] X-Plane production pipeline: golden compatibility harness vs
  convert424toxplane v12.4 (32,431/32,431 fixes value-identical;
  4,128/4,221 nav rows data-equivalent; documented divergences only)
- [x] Transactional simulator installation: journaled install_layer
  with backup/swap/post-validation/rollback, crash recovery,
  failpoint tests; CLI `export xplane --install-to`
- [x] Sim-world resolver: `resolve_sim_world` + `compare_nav_sim`
  (layer identity file, world fingerprint cross-check); CLI
  `--verify-sim`
- [x] Signed release channel: Ed25519 sign/verify with fail-closed
  trust-root handling (production keys remain a provisioning step)
- [ ] MSFS 2024 navdata support (BGL/navdata packager — future)
- [ ] Production trust root provisioning (outside the repository)

## "OpenAIRAC 1.0 ready" — definition

1.0 means an operator can run OpenAIRAC end-to-end for at least one
simulator with current AIRAC data:

- cycle discovery → preload → activation is automatic and verified;
- reconciliation coverage includes every entity class OpenAIRAC decodes;
- bundles are signed, reproducible, and installable into a simulator;
- rollback works at publication, cycle, and artifact level;
- data quality gates block releases on unresolved errors (warnings allowed);
- FAA terminal airports/runways, ILS associations, and procedure
  geometry are complete (v0.5 + v0.6 scope).

Audit state: see docs/1.0_READINESS.md (engine satisfies this
definition except production signing trust-root provisioning).

## Future (separate product/application)

- **OpenAIRAC Flight Deck / EFB** — developed separately only after the
  OpenAIRAC navdata engine is mature. Not part of any engine milestone.
- Little Navmap connector (engine maturity permitting)
