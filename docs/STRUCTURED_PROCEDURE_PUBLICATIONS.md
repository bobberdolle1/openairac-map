# Structured Procedure Publications vs Graphical Charts

## 1. Why Structured Publications Matter

Many national civil aviation authorities do not publish raw ARINC 424 or AIXM 5.x procedure datasets for free public download, but **do publish explicit tabular procedure coding requirements** within their electronic Aeronautical Information Publications (eAIP).

```text
Official AIP Section AD 2.24
         │
         ├───> Graphical Chart Plate (PDF/PNG) ──> HumanReadableChart ──> Visual Reference Only
         │
         └───> DATA Coding Table (PDF/HTML) ───> StructuredProcedurePublication ──> Canonical Navdata
```

---

## 2. Table-to-ARINC Mapping Standard

| Published Column Header | Canonical Field | Standard Interpretation |
| :--- | :--- | :--- |
| `IDENT` / `SID` / `STAR` | `procedure_ident` | Full procedure name (e.g. `OPALE 5A`) |
| `SEQ` / `STEP` | `sequence_number` | Ordered sequence execution (10, 20, 30...) |
| `PATH` / `TERMINATOR` | `path_terminator` | ARINC 424 Path Terminator (`IF`, `TF`, `CF`, `DF`) |
| `FIX` / `WAYPOINT` | `fix_ident` | Designated point / runway threshold |
| `OVER` / `FLY-OVER` | `is_flyover` | `Y` = Fly-Over, `N` = Fly-By |
| `DIR MAG` / `CRS MAG` | `course_mag_deg` | Magnetic track / course in degrees |
| `DIST` | `distance_nm` | Distance in nautical miles |
| `TURN` | `turn_direction` | Required turn direction (`L` / `R`) |
| `ALTITUDE` | `altitude_constraint` | `+` (At/Above), `-` (At/Below), `B` (Between), `At` |
| `MAX IAS` / `SPEED` | `speed_constraint` | Speed limit constraint (e.g. `250 kt`) |
| `NAV SPEC` | `route_qualifiers` | Navigation spec (`RNAV 1`, `RNP 1`, `RNP APCH`) |
