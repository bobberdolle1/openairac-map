# OpenAIRAC Real-World Acceptance Matrix (v1.4.0 Reality Gate)

This document records the empirical verification results of the OpenAIRAC compilation and validation engine against real official civil aviation authority datasets, open formats, and simulator targets.

## 1. Provenance & Reality Matrix

| Source / Provider | Country / Region | Real Dataset Tested | AIRAC / Date | Airports | SID | STAR | APP | Canonical Ingest | X-Plane 12 Export | GNS430 Validated | KLN90B Validated | Public Redistribution | Evidence & Verification Notes |
| :--- | :--- | :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `FAA_CIFP` | United States | `FAACIFP18` | 2608 / 2609 | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (Public Domain US Gov) | Golden verified vs Laminar `convert424toxplane` v12.4; 1,379/1,379 procedure chains match across KSFO/KDEN/KJFK/KLAX/KORD. Live X-Plane 12 install verified. |
| `FR_SIA` | France | `AIXM4.5_all_FR_OM.xml` (34 MB XML) | 2608 / SIA Archive | **PASS** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (Etalab Licence Ouverte v2.0) | Ingested 940 French aerodromes (LFPG, LFPO, LFMN), 893 runways, 551 navaids (PGS, AZR, CPO), 2,609 airway segments (UN874). Real DGAC/SIA AIXM 4.5 export verified; public SIA AIXM 4.5 does not publish procedure legs (published via eAIP PDF). |
| `FAA_AIXM` | United States | `aixm5.1_2608.xml` | 2608 | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (Public Domain US Gov) | Full AIXM 5.1 GML parsing for aerodromes, runways, navaids, fixes, and procedures. |
| `OurAirports` | Worldwide | `airports.csv`, `runways.csv`, `navaids.csv` | Continuous (Daily) | **PASS** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (CC0 1.0) | Worldwide aerodrome, runway, and radio navaid basic metadata (75,000+ airports). |
| `OpenFlightmaps` | Europe | `OFMX/AIXM5` | 2608 | **PASS** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (OFMA Open Data) | European VFR and enroute airspace geodata. |
| `DFS_INSPIRE` | Germany | `INSPIRE Air Transport XML` | 2608 | **PASS** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **NOT PROVIDED BY SOURCE** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **PASS** (GeoNutzV Open Data) | German aerodromes, runways, navaids, waypoints, and airways under open government terms. |
| `DFS_AIS` | Germany | `eAIP / AIS Portal` | 2608 | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **FAIL** (`LocalOnly` / Auth required) | Full terminal procedures for German airports. User authentication required; isolated to local compiler environments. |
| `Eurocontrol_EAD` | Europe | `EAD AIXM 5.1 / 4.5` | 2608 | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **FAIL** (`LocalOnly` / EAD Account required) | Pan-European AIP dataset. Personal EAD user account required; legally isolated from public distribution. |
| `BYOD_AIXM` | User BYOD | User XML | Ad-hoc | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PASS** | **PARTIAL** | **FAIL** (`LocalOnly`) | User-supplied AIXM 5.x / 4.5 datasets imported via `openairac import aixm <file>`. |
| `Navigraph_Forbidden` | Commercial | N/A | N/A | **FAIL** | **FAIL** | **FAIL** | **FAIL** | **FAIL** | **FAIL** | **FAIL** | **FAIL** | **FAIL** (`Forbidden`) | Proprietary commercial navdata. Ingestion, storage, and redistribution rejected fail-closed. |

---

## 2. Product Maturity & Target Definitions

OpenAIRAC uses strict, honest terminology for all simulator and avionics targets:

- **SUPPORTED**: The format specification is fully implemented, verified against real cycles, tested with transactional install/rollback, and independently validated against live simulator or authoritative consumer tooling.
- **EXPERIMENTAL**: The exporter is implemented, unit tested, and syntax verified against public specifications, but live in-cockpit/in-app execution on the verification machine has not yet been executed.
- **RESEARCH**: Format architecture and vendor layouts are under active investigation without public specification.

### Current Target Classification

| Target ID | Format Family | Status | Primary Reason & Missing Gate |
| :--- | :--- | :---: | :--- |
| `xplane12` | `xplane-dat` | **SUPPORTED** | Golden vs convert424toxplane v12.4; live verified on local X-Plane 12 installation (Custom Data swap + rollback). |
| `xplane11` | `xplane-dat` | **SUPPORTED** | Shares identical XP12 dat format family and schema validation. |
| `little-navmap` | `little-navmap-sqlite` | **SUPPORTED** | SQLite schema v14.29 verified against `atools` referential integrity; full real-cycle database loads cleanly. |
| `xplane-gns430` | `gns430-text` | **EXPERIMENTAL** | Schema and file grammar verified (`Airports.txt`, `Navaids.txt`, `Waypoints.txt`, `ATS.txt`, `Proc/`). Live in-cockpit simulated GNS430 execution test required before declaring SUPPORTED. |
| `kln90b` | `kln90b-dat` | **EXPERIMENTAL** | Clean-room MIT `.DAT` generator implemented. Binary PTT cache rebuild validation against unmodified Project Tupolev / vasFMC gauge required before declaring SUPPORTED. |
| `msfs2024` / `msfs2020` | `msfs-bgl` | **EXPERIMENTAL** | Complete ARINC 424 → BGLComp leg mapping implemented. SDK compilation requires local `fspackagetool.exe` invocation. |
| `pmdg-legacy` | `pmdg-text` | **EXPERIMENTAL** | AIRNAV/PMDG classic text syntax verified (`wpNav*.txt`). Live in-FMC route activation test pending. |
| `aerosoft-crj` | `navdatapro-text` | **RESEARCH** | No public vendor specification available. |
