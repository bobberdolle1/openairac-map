# French SIA Structured Procedure Publication Ingestion (v2.1.0)

## 1. Authority & Legal Terms

* **Authority**: Direction Générale de l'Aviation Civile (DGAC) / Service de l'Information Aéronautique (SIA France).
* **Licence**: **Etalab Licence Ouverte v2.0** (French Open Government Licence).
* **Terms of Reuse**: Freely permits extraction, transformation, compilation, and redistribution subject to explicit attribution:
  > *"Source: Service de l'Information Aéronautique (SIA France) under Etalab Licence Ouverte v2.0"*.
* **Classification**: `PublicRedistribution` for derived canonical procedures.

---

## 2. Ingestion Architecture (`openairac_ingest::sia_procedures`)

SIA publishes official database coding requirements in Section AD 2.24:
* `DATA SID RNAV RWY ...`: Standard Instrument Departures (e.g. `OPALE 5A`, `ATREX 5A`, `NURMO 5A` at `LFPG`).
* `DATA STAR RNAV RWY ...`: Standard Terminal Arrival Routes (e.g. `VEBEK 5E`, `PILUL 5E`, `OKTET 5E` at `LFPG`).
* `DATA RWY ... FNA RNP`: Final Approach RNP procedures (e.g. `RNP 26L`, `RNP 26R` at `LFPG`).

### Ingested Coding Fields:
* **Sequence Numbers**: Ordered execution (`010`, `020`, `030`, `040`, `050`).
* **Path Terminators**: Full ARINC 424 support (`IF`, `TF`, `CF`, `DF`, `FA`, `CA`, `VA`, `VI`, `VM`, `RF`, `HA`, `HF`, `HM`).
* **Fly-Over Semantics**: Discrete flag (`Y` = Fly-over, `N` = Fly-by).
* **Courses**: Explicit Magnetic Course (°M) and True Course (°T).
* **Distances**: Precise leg distances in nautical miles.
* **Turn Directions**: Explicit turn constraints (`L` / `R`).
* **Altitude Constraints**: Window constraints (`Between`), minimums (`AtOrAbove`), maximums (`AtOrBelow`), exact altitudes (`At`).
* **Speed Limits**: Maximum IAS in knots (e.g. `MAX 250 kt`).
* **Navigation Specifications**: `RNAV 1`, `RNP 1`, `RNP APCH`.
* **Vertical Navigation**: Vertical Path Angle (°), Threshold Crossing Height (ft).

---

## 3. Scoped Terminal Fixes

Fix coordinates published in French eAIP DATA tables are mapped with scoped terminal provenance (e.g. `PG261`, `PG262`, `PG081`, `PG082` at LFPG; `PO061` at LFPO; `MN041` at LFMN).
