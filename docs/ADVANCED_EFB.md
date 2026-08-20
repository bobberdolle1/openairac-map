# OpenAIRAC Map — Advanced EFB Architecture (v0.5.0)

## 1. Overview & Vision

OpenAIRAC Map v0.5.0 transforms into a full-featured **Electronic Flight Bag (EFB)** that unites navigation data, official charts, live weather, online traffic, and simulator telemetry into a cohesive, phase-aware cockpit workflow.

```text
+-----------------------------------------------------------------------------+
|                           OPENAIRAC ADVANCED EFB                            |
+-----------------------------------------------------------------------------+
|  PREFLIGHT   | Airport Workspace (Overview, Weather, Charts, VATSIM, Runways)|
|  TAXI OUT    | Airport Moving Map + Departure Airport Diagram Suggestion    |
|  DEPARTURE   | Active SID Procedure & Chart Auto-Suggestion                 |
|  ENROUTE     | Active Flight Progress, XTK, Planning TOD, Corridor Traffic  |
|  ARRIVAL     | Active STAR Procedure & Destination Weather Update           |
|  APPROACH    | IAP Chart Suggestion, Destination ATIS, Runway Crosswind     |
|  TAXI IN     | Destination Airport Moving Map & Parking Diagram             |
+-----------------------------------------------------------------------------+
```

---

## 2. Core EFB Modules

* **`AirportWorkspace`**: Unified dock providing 7 comprehensive panels for any airport (`Overview`, `Weather`, `Charts`, `Procedures`, `Online`, `Runways & Wind`, `Ground Map`).
* **`FlightPhaseEngine`**: Deterministic state machine tracking the real-time flight phase from telemetry.
* **`ChartContextEngine`**: Phase-aware chart suggestions and automatic chart display modes (`Off`, `Suggest`, `AutoOpen`).
* **`ActiveFlightDock`**: Flight progress dashboard showing active/next legs, distance/ETA, cross-track error, planning TOD, and destination operational intelligence.
* **`GeoRasterLayer`**: Calibrated raster map layer supporting official FAA GeoTIFF charts with live ownship overlays.
