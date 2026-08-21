# OpenAIRAC Worldwide Aeronautical Data Architecture

## 1. Executive Summary

OpenAIRAC is an open, deterministic, worldwide aeronautical navigation data compiler, federation engine, and validator. Rather than relying on a single proprietary supplier (such as Navigraph or Jeppesen), OpenAIRAC federates official civil aviation authority datasets (FAA, Eurocontrol, DFS, national AIPs), open community registries (OurAirports, open flightmaps), and Bring-Your-Own-Data (BYOD) sources into a unified, temporal, canonical world database.

```text
+-------------------------------------------------------------------------------+
|                        Official & Open Data Sources                           |
|  [FAA CIFP/AIXM]    [OurAirports]    [open flightmaps]    [DFS/EAD/BYOD]      |
+-------------------------------------------------------------------------------+
                                       │
                                       ▼
+-------------------------------------------------------------------------------+
|                   Provenance, Licensing & Policy Layer                        |
|  - Redistribution Permissions (Public, Local-Only, Metadata-Only, Forbidden)  |
|  - Machine-readable registry (data/providers.yaml)                            |
|  - Content-addressed source snapshot cryptographic verification               |
+-------------------------------------------------------------------------------+
                                       │
                                       ▼
+-------------------------------------------------------------------------------+
|                     Canonical Temporal World Store                            |
|  - Airports, Runways, Navaids, Waypoints, Airways, Procedures, FAS, MSA, MORA |
|  - Temporal validity envelopes: world_at(t)                                   |
|  - Multi-source entity reconciliation & conflict detection                    |
+-------------------------------------------------------------------------------+
                                       │
                                       ▼
+-------------------------------------------------------------------------------+
|                    Worldwide Procedure Validation Engine                      |
|  - Fix resolution, sequence continuity, runway binding, gradient checks       |
|  - Geometric jump and path discontinuity detection                            |
+-------------------------------------------------------------------------------+
                                       │
                                       ▼
+-------------------------------------------------------------------------------+
|                        Multi-Target Simulator Exporters                       |
|  [X-Plane 12/11]   [GNS430 Navdata]   [KLN90B]   [MSFS BGL]   [Little Navmap] |
+-------------------------------------------------------------------------------+
```

## 2. Core Pillars

### 2.1 Never Invent Navdata
OpenAIRAC strictly enforces that absent data is explicitly recorded as missing or unrepresented. Synthesizing plausible fictitious procedures or guessed coordinates is prohibited.

### 2.2 Uncompromising Provenance
Every entity in the store carries a direct foreign key to a `source_snapshot` containing the provider name, dataset identifier, content SHA-256 hash, retrieval timestamp, confirmed effective window, and SPDX/legal license identifier.

### 2.3 Explicit Licensing & Distribution Policy
Datasets are categorized by redistribution rights:
- **Public Redistribution**: Open datasets (e.g. FAA Public Domain, OurAirports CC0) safely bundled into official release packages.
- **Local-Only (BYOD)**: User-provided or authenticated datasets (e.g. Eurocontrol EAD, private AIP downloads) permitted for local compilation on user machines, but cryptographically blocked from inclusion in public release bundles.
- **Forbidden**: Proprietary commercial datasets (Navigraph, Jeppesen, NavDataPro). Ingestion, redistribution, or repository inclusion is rejected fail-closed.

### 2.4 Generic AIXM 5.x Integration
AIXM 5.1/5.1.1 XML/GML ingestion transforms aerodromes, runways, navaids, designated points, routes, and terminal procedures (SIDs, STARs, Approaches, Transitions, Legs) into canonical entities with zero data loss.

### 2.5 Worldwide Procedure Validation
Before any procedure is rendered or exported, it passes structural, semantic, and geometric validation:
- Referenced fixes and navaids are resolved to coordinates.
- Leg sequences are verified for continuity and monotonically increasing order.
- Runway transitions are bound to existing aerodrome runways.
- Leg geometry (RF arc radius, turn direction, hold inbound course) is verified.
- Monotonicity of altitude profiles (SIDs climbing, STARs/Approaches descending) and realistic airspeed constraints (60–400 kts) are enforced.
- Discontinuity jumps (> 250 NM in terminal areas) are flagged as critical errors.

## 3. Simulator Exporter Matrix

| Target ID | Format Family | Simulator / Addon | Maturity |
| :--- | :--- | :--- | :--- |
| `xplane12` | `xplane-dat` | X-Plane 12 Custom Data | **SUPPORTED** |
| `xplane11` | `xplane-dat` | X-Plane 11 Custom Data | **SUPPORTED** |
| `xplane-gns430` | `gns430-text` | Garmin GNS430 / Classic GPS | **SUPPORTED** |
| `kln90b` | `kln90b-dat` | Bendix/King KLN90B (Tu-154, vasFMC) | **SUPPORTED** |
| `little-navmap` | `little-navmap-sqlite` | Little Navmap Flight Planner | **SUPPORTED** |
| `msfs2024` | `msfs-bgl` | Microsoft Flight Simulator 2024 | **EXPERIMENTAL** |
| `msfs2020` | `msfs-bgl` | Microsoft Flight Simulator 2020 | **EXPERIMENTAL** |
| `pmdg-legacy` | `pmdg-text` | PMDG Classic FMC | **EXPERIMENTAL** |
| `aerosoft-crj` | `navdatapro-text` | Aerosoft CRJ / NavDataPro | **RESEARCH** |
