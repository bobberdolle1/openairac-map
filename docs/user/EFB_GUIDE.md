# OpenAIRAC 3.0 — Operational Electronic Flight Bag (EFB) User Guide

## 1. Overview & Architecture

**OpenAIRAC 3.0** is an open-source, subscription-free desktop Electronic Flight Bag (EFB) and flight planning suite for modern flight simulation (X-Plane 12/11, MSFS).

The application integrates two core engines:
- **OpenAIRAC Core**: Authoritative engine for navigation provider federation, canonical temporal AIRAC database, multi-identity aerodrome resolution, route planning, runway-compatibility validation, and deterministic avionics exporters.
- **OpenAIRAC Map**: Desktop user interface, high-performance vector moving map, airport workspace, chart viewer, weather telemetry, online traffic monitor, and simulator connection hub.

---

## 2. Flight Planning & Dynamic Procedure Selection

### Workflow:
1. **Select Origin & Destination**: Enter any verified ICAO, IATA, or regional identifier (e.g. `UUEE`, `URFF`, `URAS`, `LFPG`, `LFMN`, `KJFK`).
2. **Select Aircraft Profile**: Choose an operational profile (e.g. `TU154`, `A320`, `B738`, `B744`, `C172`, `IL76`, `IL96`, `AN24`, `YK40`). Profiles enforce realistic runway length, width, and surface constraints.
3. **Choose Planning Mode**:
   - **Strict ATS**: Mandatory published airway network traversal.
   - **Normal / Hybrid**: Optimized airway corridors with departure/arrival joins.
   - **Allow DCT**: Geodesic direct connections where published airways are absent.
4. **Dynamic Runway & Terminal Procedure Selection**:
   - The planner automatically filters SIDs, STARs, and instrument approaches that are strictly compatible with your assigned departure and arrival runways (e.g. UUEE RW24C pairs with `EMGAS 3H`, RW06C pairs with `EMGAS 3E`).
   - Selecting an incompatible runway or wrong procedure role (e.g. requesting a STAR as a SID) is rejected as **INVALID** to protect flight safety.
5. **Review & Save**: Inspect the ordered route phases (`DEP`, `SID`, `ENROUTE`, `DCT`, `STAR`, `APP`, `MISSED`) and save to your flight library.

---

## 3. Provider Platform & Local AIP Vault

OpenAIRAC sources navigation data exclusively from official government civil aviation authorities and public datasets:
- **FAA Aeronautical Information Services**: Complete US CIFP terminal procedures, airway routes, and fixes (Public Domain).
- **DGAC France / SIA**: French AIP aerodromes, runways, radio navaids, and route network (Licence Ouverte v2.0).
- **DFS Germany**: Airspace and navigation geometry (GeoNutzV-OpenData).
- **OurAirports**: Worldwide baseline aerodromes and runways (Public Domain).
- **CAICA Russia / Rosaviatsiya**: Official Russian Federation AIP & AIM No. 12 structured procedure tables (Local AIP Vault).

### Importing Official Local Data:
1. Open **OpenAIRAC > Data & Providers Manager...** (`Ctrl+Alt+D`).
2. Select **CAICA Russia** and click **Import Local Package**.
3. Choose your local official dataset. The Local AIP Vault validates the payload, verifies cycle integrity, and atomically activates the dataset without corrupting public baseline data.

---

## 4. Multi-Identity Aerodrome Model

OpenAIRAC treats aerodrome infrastructure as a physical reality decoupled from political and provider designation changes:
- **Simferopol International**: Single physical aerodrome mapped to CAICA (`URAS`/`URFF`), OurAirports (`UKFF`), and IATA (`SIP`).
- **Sukhumi Babushara**: Physical facility mapped to CAICA (`URAS`), OurAirports (`UGSS`/`SUI`/`UG29`), and Georgia AIS (`NotListedInProvider`).
- **Pskhu Airfield**: Baseline identifier `GE-0015` with collision safeguards against Gudauta (`UGSG`) and Bolshiye Shiraki (`UG28`).
- **Source Depth Transparency**: Where detailed instrument procedure datasets are not publicly distributable (e.g. Sukhumi terminal procedures), procedures are marked **SOURCE REQUIRED** with zero fabricated data.

---

## 5. Weather, Online Flying & Live Tracking

- **Live Surface Weather**: Real-time METAR and TAF telemetry from NOAA AviationWeather.gov with automated runway headwind/crosswind calculation.
- **VATSIM & IVAO Network Traffic**: Real-time online pilot positions, active ATC controllers, and live ATIS broadcasts displayed directly on the moving map.
- **Simulator Connection**: Seamless connection to X-Plane 12 and MSFS via native UDP/SimConnect protocols.
- **Live Aircraft Progress**: Real-time position, ground speed, true heading, altitude, distance flown, distance remaining, Top-of-Descent (TOD), and cross-track deviation (XTK).

---

## 6. Avionics & Simulator Export

Export flight plans to multiple avionics and simulator formats with zero manual retyping:
- **X-Plane 11/12 FMS (`.fms`)**: Full modern 1100-version flight plans including departure runway, SID, enroute legs, STAR, and Approach.
- **Garmin GNS430 / GNS530 (`.fpl`)**: Formatted flight plans compatible with legacy GPS units.
- **Bendix/King KLN90B (`.dat`)**: Route records for classic aircraft navigation systems.

---

## 7. Quality Gates: FORMAT PASS vs LIVE TESTED

- **FORMAT PASS**: The export payload syntax, path terminators, sequence numbers, and altitude constraints have been verified against formal grammar specifications and automated consumer parsers.
- **LIVE TESTED**: The flight plan or scenery has been interactively loaded, verified, and flown inside a live, running simulator binary (e.g. X-Plane 12).
