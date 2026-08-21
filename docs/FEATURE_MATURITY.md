# OpenAIRAC Target & Feature Maturity Matrix (v2.0.0)

## 1. Feature Maturity Classification

| Target / Subsystem | Format Family | Maturity Status | Verification Gate |
| :--- | :--- | :---: | :--- |
| **X-Plane 12** | `xplane-dat` (`XPNAV1200`, `XPFIX1200`, `XPAWY1101`, `MORAXP1150`, `MSAXP1150`, LPV FAS rows 14 & 16) | **SUPPORTED** | 100% agreement against Laminar `convert424toxplane` v12.4 on real AIRAC cycles; verified on local X-Plane 12. |
| **OpenAIRAC Map** | `little-navmap-sqlite` (Schema v14.29) | **SUPPORTED** | End-to-end flight planning, moving map, Airport Workspace, weather, charts, and VATSIM awareness. |
| **Aviation Weather** | NOAA AviationWeather.gov REST API | **SUPPORTED** | Real-time global METAR, TAF, international SIGMET polygons, and PIREP ingestion. |
| **Online Flight Simulation** | VATSIM Data API v3 & Events API v2 | **SUPPORTED** | 1,800+ concurrent clients, pilot motion interpolation, ATC range markers, decoded ATIS broadcasts. |
| **Geospatial Rasters** | FAA GeoTIFF 6-Parameter Affine Transform | **SUPPORTED** | Reversible coordinate transforms with $\le 0.001\text{ px}$ round-trip validation. |
| **Garmin GNS430** | `gns430-text` (`Airports.txt`, `Navaids.txt`, `Proc/`) | **EXPERIMENTAL** | Complete text grammar and procedure serialization verified. |
| **KLN90B GPS** | `kln90b-dat` (`APT.DAT`, `NAV.DAT`, `FAS.DAT`) | **EXPERIMENTAL** | Clean-room MIT `.DAT` serialization verified. |
| **MSFS 2024 / 2020** | `msfs-bgl` (BGLComp Package XML) | **EXPERIMENTAL** | Full ARINC 424 to BGLComp leg translation (199,966 legs on cycle 2609). Requires local `fspackagetool.exe` compilation. |
| **PMDG Classic** | `pmdg-text` (`wpNavAPT`, `wpNavAID`, `wpNavFIX`, `wpNavRTE`) | **EXPERIMENTAL** | AIRNAV/PMDG classic text syntax verified. |
