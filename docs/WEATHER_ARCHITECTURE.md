# OpenAIRAC Weather Architecture

This document describes the design, domain models, caching strategy, security safeguards, and route corridor hazard analysis engine of the **OpenAIRAC Weather Subsystem** (`openairac-weather`).

---

## 1. Domain Decoupling & Temporal Semantics

A core architectural principle in OpenAIRAC is the separation of long-lived cyclic navigation data from dynamic temporal weather observations:

```text
┌─────────────────────────────────────────────────────────────┐
│                       OpenAIRAC Platform                    │
├─────────────────────────────┬───────────────────────────────┤
│    Cyclic AIRAC Navdata     │     Dynamic Weather Layer     │
│   (28-Day Periodic Cycle)   │    (Minutes / Hourly TTL)     │
├─────────────────────────────┼───────────────────────────────┤
│  - Airports & Runways       │  - Live Surface METARs        │
│  - Radio Navaids & Fixes    │  - Terminal Aerodrome TAFs    │
│  - Airway Network Segments  │  - International SIGMETs      │
│  - Standard Procedures      │  - PIREP Pilot Reports        │
│  - LPV FAS Geometric Blocks │  - Winds Aloft & Temperatures │
└─────────────────────────────┴───────────────────────────────┘
```

**Fundamental Weather Invariant**: Every weather record carries explicit observation/issue timestamps, valid periods, fetch time, and data staleness (`Fresh`, `Aging`, `Stale`, `Expired`). Stale or unavailable data is never presented as current.

---

## 2. Core Domain Models (`openairac-weather::model`)

- `FlightCategory`: Decoded flight category (`VFR`, `MVFR`, `IFR`, `LIFR`, `UNKNOWN`) based on ceiling (ft AGL) and visibility (SM).
- `MetarReport`: Surface weather observation with temperature, dewpoint, wind direction/speed/gusts, visibility, altimeter (hPa / inHg), cloud layers, and weather phenomena.
- `TafReport` & `TafForecastPeriod`: Multi-period terminal aerodrome forecasts with change types (`FM`, `TEMPO`, `PROB30`, `BECMG`) and `forecast_at_eta(eta)` resolution.
- `Sigmet`: International and domestic polygon advisories for convective storms (`TS`), severe turbulence (`TURB`), severe icing (`ICE`), volcanic ash (`VA`), and tropical cyclones (`TC`).
- `PirepReport`: In-flight pilot reports with altitude, turbulence, icing, and ambient temperature.

---

## 3. Route Corridor & Hazard Intersection (`openairac-weather::corridor`)

The `RouteCorridor` engine evaluates active flight plans against weather hazards:
- **Corridor Geometry**: Configurable lateral buffer (default 50 NM half-width = 100 NM total corridor width) along geodesic great-circle route segments.
- **Intersection Testing**:
  1. Vertex containment: Ray-casting point-in-polygon tests for all route waypoints.
  2. Proximity tests: Great-circle distance calculations from polygon vertices to route segments.
  3. Edge intersections: 2D segment-to-segment crossing tests.
- **Deterministic Reporting**: Returns all active SIGMET polygons and PIREPs intersecting the flight corridor without guessing or artificial AI fabrication.

---

## 4. Isolated Cache Storage (`openairac-weather::cache`)

Dynamic weather payloads are stored in an isolated SQLite database (`openairac_weather.sqlite`):
- Explicit TTLs per product (15 min for METAR, 60 min for TAF, validity-based for SIGMETs).
- Automatic pruning of expired records.
- Zero contamination of Little Navmap navigation database files (`openairac.sqlite` and `little_navmap_navigraph.sqlite`).
