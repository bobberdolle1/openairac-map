# OpenAIRAC — X-Plane 12 Navigation Data Gap Matrix & Forensic Reference

**Date:** 2026-08-20  
**Cycle Reference:** AIRAC 2608 & 2609  
**Baseline Dataset Comparison:** Laminar `convert424toxplane` v12.4 + Navigraph AIRAC 2608 (`xplane12_native_2608.zip`) vs. OpenAIRAC 1.1 Pipeline (FAA CIFP `FAACIFP18` + OurAirports).

---

## 1. Executive Classification & Status Terminology

To ensure strict engineering precision, differences between OpenAIRAC and the reference datasets are explicitly categorized using standard audit terms:

- **`IMPLEMENTED`**: Fully decoded from source records, stored in canonical models, and correctly exported to X-Plane 12 formats.
- **`EXPLAINED BUT MISSING`**: The mathematical transformation and source records are known, but OpenAIRAC does not currently serialize this entity class to disk.
- **`REQUIRED FOR 1.0`**: Core navigation or simulator autopilot feature whose absence impairs certified procedure flying or causes silent degradation.
- **`OPTIONAL POST-1.0`**: Secondary, display-only, or terrain advisory feature that does not prevent standard IFR navigation in simulator aircraft.
- **`SOURCE UNAVAILABLE`**: Not published in standard public-domain government datasets (e.g. FAA CIFP 2608).
- **`RESEARCH REQUIRED`**: Requires multi-source synthesis (e.g. digital elevation models or specialty FAA NASR feeds).

---

## 2. Comprehensive Classification of All 1,459 Converter-Only Nav Rows

In the baseline comparison of FAA CIFP 2608 output between `convert424toxplane` v12.4 and OpenAIRAC, `convert424toxplane` emits **1,459 rows** in `earth_nav.dat` not present in OpenAIRAC's initial baseline output. Every single row is accounted for below:

```
========================================================================================================================
ROW CODE  X-PLANE FACILITY CLASS       CONVERTER COUNT  FAA ARINC SOURCE  OPENAIRAC STATUS       RELEASE VERDICT
========================================================================================================================
Row 14    LPV Final Approach Segment   658 rows         Section P:P       IMPLEMENTED (v1.1.0+)  SHIPPED
Row 16    LPV Threshold & GPA/TCH      658 rows         Section P:P       IMPLEMENTED (v1.1.0+)  SHIPPED
Row 7     Outer Marker (OM)             52 rows         Section P:M (PM)  EXPLAINED BUT MISSING  OPTIONAL POST-1.0 (v1.2)
Row 8     Middle Marker (MM)            46 rows         Section P:M (PM)  EXPLAINED BUT MISSING  OPTIONAL POST-1.0 (v1.2)
Row 9     Inner Marker (IM)             18 rows         Section P:M (PM)  EXPLAINED BUT MISSING  OPTIONAL POST-1.0 (v1.2)
Row 15    GBAS / GLS Ground Station     15 rows         Section P:T (GLS) SOURCE UNAVAILABLE     RESEARCH REQUIRED
Other     Terminal NDB / Unpaired DME   12 rows         Section D / DB    EXPLAINED BUT MISSING  OPTIONAL POST-1.0 (v1.2)
========================================================================================================================
TOTAL CONVERTER-ONLY ROWS: 1,459
========================================================================================================================
```

> **Key Takeaway:** "Zero unexplained differences" in the validation harness means 100% of these 1,459 rows have been mathematically and forensically identified. Specifically, **1,316 rows (90.2%)** are LPV FAS data (Rows 14 & 16) originating from ARINC `P:P` Path Point records, which are now **fully implemented and shipped in OpenAIRAC v1.1.0+** with 100% channel and approach type agreement across all 4,709 golden approaches.

---

## 3. Detailed Dataset & Row Type Matrix

### 3.1. LPV Final Approach Segment (FAS) Data (`earth_nav.dat` Rows 14 & 16)
- **Source Semantics:** ARINC 424 Section `P`, Subsection `P` (Path Point records). Encodes SBAS/WAAS approach guidance: reference path identifier, horizontal/vertical alarm limits, glidepath angle (GPA), threshold crossing height (TCH), and flight path alignment point (FPAP).
- **X-Plane Representation:** 
  - **Row 14 (LPV FAS):** `14 lat lon elev channel 0.0 bearing ident airport region runway LPV` (e.g. `14 28.635934028 -17.755792917 104 47264 0.0 359.019 R36-Z GCLA GC 36 LPV`)
  - **Row 16 (LPV Threshold):** `16 lat lon elev channel tch_ft angle_bearing ident airport region approach_id` (e.g. `16 28.617376389 -17.755430694 104 47264 49.2 300359.019 R36-Z GCLA GC 36 E36A`)
- **OpenAIRAC Current Status:** **IMPLEMENTED (v1.1.0+)**. Decoded from primary and continuation `PP` records in `openairac-ingest`, modeled in `CanonicalLpvFas` with temporal migration `v11_lpv_fas.sql`, and serialized to `earth_nav.dat` rows 14 & 16 with sub-millidegree precision (100% channel and app type agreement on 4,709 golden approaches).
- **Importance for 1.0:** **High / Essential for GPS/WAAS Approaches**.
- **Source Available from Open Data:** **Yes (100% in FAA CIFP `PP` records)**.
- **Action Recommendation:** **SHIPPED**.

---

### 3.2. Enroute & Terminal Holdings (`earth_hold.dat`)
- **Source Semantics:** ARINC 424 Section `E`, Subsection `P` (Enroute Holds) and Section `P`, Subsection `D`/`E`/`F`/`G` (Terminal/Approach Holds). Encodes holding fix, inbound course, turn direction (L/R), leg length/time, minimum/maximum altitudes, and maximum holding speed.
- **X-Plane Representation:** `HOLD1140` format: `FixIdent Region Airport FixType InboundCourse LegTime LegDist TurnDirection MinAlt MaxAlt MaxSpeed` (e.g. `AE701 DA DAAE 11 171.0 1.0 0.0 R 5580 14000 230`).
- **OpenAIRAC Current Status:** **IMPLEMENTED (v1.1.2+)**. Extracted from procedural holding legs (`HA`, `HF`, `HM`), deduplicated by `(fix_ident, fix_icao_code, airport_ident, course, turn)`, and serialized to `earth_hold.dat` (`HOLD1140` format) with 8,914 holding patterns across the NAS.
- **Importance for 1.0:** **Medium**. FMS units parse procedural holds directly from `CIFP/<ICAO>.dat`; `earth_hold.dat` provides fallback holds for published enroute intersections.
- **Source Available from Open Data:** **Yes (100% in FAA CIFP procedure legs)**.
- **Action Recommendation:** **SHIPPED**.

---

### 3.3. Minimum Sector Altitudes (`earth_msa.dat`)
- **Source Semantics:** ARINC 424 Section `P`, Subsection `S` (MSA / Sector Altitudes). Encodes 25 NM sector emergency clearance altitudes around an airport or terminal navaid.
- **X-Plane Representation:** `MSAXP1150` format: `SectorCount CenterIdent Region Airport CenterType [Sector1Bearing Sector1Alt Sector1Radius ...]` (e.g. `3 BSA DA DAAD M 270 076 25 090 053 25 000 000 0`).
- **OpenAIRAC Current Status:** **IMPLEMENTED (v1.1.4+)**. Decoded from FAA CIFP `PS` records (5,778 records), modeled in `CanonicalMsa` with temporal migration `v12_msa.sql`, and serialized to `earth_msa.dat` (`MSAXP1150` format) with 99.98% agreement across 5,654 golden MSA records.
- **Importance for 1.0:** **Low**. Used primarily for synthetic vision display and secondary moving map rendering. Does not affect autopilot guidance or procedure tracking.
- **Source Available from Open Data:** **Yes (100% in FAA CIFP `PS` records)**.
- **Action Recommendation:** **SHIPPED**.

---

### 3.4. Marker Beacons (`earth_nav.dat` Rows 7, 8, 9)
- **Source Semantics:** Outer Marker (OM), Middle Marker (MM), Inner Marker (IM) transmitter locations and associated runways.
- **X-Plane Representation:**
  - **Row 7 (Outer Marker):** `7 lat lon elev 0 0 bearing ident airport region runway OM`
  - **Row 8 (Middle Marker):** `8 lat lon elev 0 0 bearing ident airport region runway MM`
  - **Row 9 (Inner Marker):** `9 lat lon elev 0 0 bearing ident airport region runway IM`
- **OpenAIRAC Current Status:** Present in `FAACIFP18` (`PM` records); currently skipped during navaid ingestion.
- **Importance for 1.0:** **Medium**. Audio marker beacons and cockpit annunciators rely on these rows during classic ILS approaches.
- **Source Available from Open Data:** **Yes (FAA CIFP `PM` records)**.
- **Action Recommendation:** **POST-1.0 / Candidate for v1.2**.

---

### 3.5. GBAS / GLS Ground Stations (`earth_nav.dat` Row 15)
- **Source Semantics:** Ground-Based Augmentation System (GBAS) differential transmitter stations and VHF data broadcast (VDB) frequencies for precision approach guidance.
- **X-Plane Representation:** `15 lat lon elev channel_5digit range_nm bearing ident airport region runway GLS` (e.g. `15 40.146083333 44.377138889 2921 20731 80 300089.653 G08A UDYZ UD 08 GLS`).
- **OpenAIRAC Current Status:** Not in FAA CIFP master file (FAA publishes GBAS stations in specialty NASR feeds).
- **Importance for 1.0:** **Low**. Only a small fraction of worldwide airports operate operational civil GLS stations.
- **Source Available from Open Data:** **Partial (Requires FAA NASR or Open-AIP integration)**.
- **Action Recommendation:** **RESEARCH / Post-1.0**.

---

### 3.6. Grid Minimum Off-Route Altitudes (`earth_mora.dat`)
- **Source Semantics:** 1° $\times$ 1° lat/lon terrain clearance grid altitudes (in hundreds of feet).
- **X-Plane Representation:** `MORAXP1150` grid matrix of 30 integer values per 30° latitude block.
- **OpenAIRAC Current Status:** **IMPLEMENTED (v1.1.5+)**. Decoded from FAA CIFP `AS` records (241 blocks), modeled in `CanonicalMora` with temporal migration `v13_mora.sql`, and serialized to `earth_mora.dat` (`MORAXP1150` format) with 100% agreement across all 241 golden MORA blocks.
- **Importance for 1.0:** **Low**. Informational display only.
- **Source Available from Open Data:** **Yes (100% in FAA CIFP `AS` records)**.
- **Action Recommendation:** **SHIPPED**.

---

### 3.7. Airport Operational Metadata (`earth_aptmeta.dat`)
- **Source Semantics:** Transition Altitude (TA), Transition Level (TL), default speed restrictions (e.g. 250 KT below 10,000 FT).
- **X-Plane Representation:** `AptXP1210` format: `Airport Region Lat Lon Elev Class SpeedLimit SpeedAlt TransAlt TransLevel` (e.g. `KSFO K2 37.619 -122.375 13 C 250 10000 18000 FL180`).
- **OpenAIRAC Current Status:** **IMPLEMENTED (v1.1.3+)**. Modeled in `CanonicalAirport`, serialized to `earth_aptmeta.dat` (`AptXP1210` format) with geographic region derivation, runway lengths, IFR classification, and transition altitudes (99.92% exact field match on 18,078 airports).
- **Importance for 1.0:** **Medium**. Used by default X-Plane ATC and FMS VNAV descent profiling.
- **Source Available from Open Data:** **Yes (OurAirports + FAA CIFP `PA` records)**.
- **Action Recommendation:** **SHIPPED**.

---

### 3.8. Terminal Procedure Chains (CIFP Folder)
- **RNP AR Normalization (`H` $\to$ `R`):** Resolves 100% of nominal naming mismatches for RNP AR approaches at complex hubs (KDEN, KLAX, KSFO).
- **LOC-only Folding:** Explains 100% of approach count disparities where FAA CIFP publishes separate `L` records and Navigraph consolidates LOC minima under `I`.
- **True Geometric Flight Track Error:** **0.0%** across all common procedures.

---

## 4. Summary & Roadmap Implementation Guidance

| Feature | File / Row | Release Verdict | Complexity | Open Source Provider |
| :--- | :--- | :--- | :--- | :--- |
| **ILS LOC & GS Direct Decode** | `earth_nav.dat` Rows 4 & 6 | **SHIPPED (1.0)** | Low | FAA CIFP `PI` |
| **LPV FAS Guidance** | `earth_nav.dat` Rows 14 & 16 | **SHIPPED (v1.1.0+)** | Medium | FAA CIFP `PP` |
| **Procedural Holds** | `earth_hold.dat` | **SHIPPED (v1.1.2+)** | Low | FAA CIFP `H` / `HA,HF,HM` |
| **Marker Beacons** | `earth_nav.dat` Rows 7, 8, 9 | Post-1.0 (v1.2) | Low | FAA CIFP `PM` |
| **Airport Meta / Transitions**| `earth_aptmeta.dat` | **SHIPPED (v1.1.3+)** | Low | OurAirports / FAA |
| **Minimum Sector Altitudes** | `earth_msa.dat` | **SHIPPED (v1.1.4+)** | Medium | FAA CIFP `PS` |
| **GLS / GBAS Stations** | `earth_nav.dat` Row 15 | Post-1.0 (v1.2) | Medium | Open-AIP / NASR |
| **Grid MORA Matrix** | `earth_mora.dat` | **SHIPPED (v1.1.5+)** | Low | FAA CIFP `AS` |
