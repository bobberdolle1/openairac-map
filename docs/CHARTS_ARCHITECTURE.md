# OpenAIRAC Charts Architecture

This document describes the design, domain models, caching mechanisms, security invariants, and provider architecture of the **OpenAIRAC Charts Subsystem** (`openairac-charts`).

---

## 1. Safety & Domain Decoupling

A foundational principle of OpenAIRAC is the strict separation between:
1. **Machine-Readable Navigation Data** (`CanonicalProcedureLeg`, `CanonicalWaypoint`, `CanonicalAirwayLeg`): Geometric constraints, path terminators, vertical angles, and speed limits consumed by FMC and autopilot flight guidance.
2. **Aeronautical Chart Documents** (`ChartDocument`, `ChartAsset`): Visual reference documents (PDF, PNG) published by national civil aviation authorities for pilot situational awareness.

```text
┌─────────────────────────────────────────────────────────────┐
│                       OpenAIRAC Platform                    │
├─────────────────────────────┬───────────────────────────────┤
│    Canonical Navdata        │        Open Charts            │
│  (ARINC 424 / AIXM 5.x)     │  (FAA d-TPP / SIA eAIP)       │
├─────────────────────────────┼───────────────────────────────┤
│  - Airport/Runway Geometry  │  - Airport Diagrams           │
│  - Navaids & Fixes          │  - SID Departure Plates       │
│  - Airway Network Segments  │  - STAR Arrival Plates        │
│  - SIDs / STARs / Approaches│  - Instrument Approach Plates │
│  - FAS Data Blocks / LPV    │  - Minima & Hotspots          │
└─────────────────────────────┴───────────────────────────────┘
                              │ Reference Link Only (Exact / Likely)
                              ▼
                 Procedure-to-Chart Association
```

**Inviolable Rule**: OpenAIRAC never performs PDF OCR or guesses navigation legs from chart graphics. If an airport (e.g. `LFPG`) has published eAIP charts but the open public AIXM data source contains zero machine-readable procedures, OpenAIRAC honestly presents charts while navdata procedures remain absent.

---

## 2. Domain Entities (`openairac-charts::model`)

- `ChartDocument`: Canonical metadata record storing title, airport ICAO/IATA, normalized category, provider-specific codes, runway hints, AIRAC cycle, validity intervals, source URLs, and redistribution licenses.
- `NormalizedChartType`: Standardized categories:
  - `AirportDiagram` (e.g. APD, ADC)
  - `ParkingDocking` (e.g. APDC, PDC)
  - `GroundMovement` (e.g. GMC)
  - `Sid` (e.g. DP, SID)
  - `Star` (e.g. STAR)
  - `Approach` (e.g. IAP, IAC)
  - `ApproachVisual` (e.g. VAC, CVFP)
  - `TakeoffMinima` / `AlternateMinima` / `RadarMinima` / `HotSpot` / `GeneralInfo`
- `ChartAssociation`: Non-invasive reference link between a `CanonicalProcedureLeg` and a `ChartDocument` with confidence rating (`Exact`, `Likely`, `Ambiguous`, `Unmatched`).

---

## 3. Storage Architecture & Cache Invariants (`openairac-charts::cache`)

Chart assets (PDFs) are stored in an isolated, content-addressed on-disk cache:

```text
charts_cache/
  └── sha256/
      ├── c5/
      │   └── c5906b87941e352e926761b407d1968d8c7288899b4face545a70c334634f477.pdf
      └── d1/
          └── d1251e7dab414a416d0898200f2d2c443b9066ee653284c9778f2232d06e2322.pdf
```

### Security Invariants
1. **Content-Addressed Hashing**: Files are verified and indexed by SHA-256 hash.
2. **Atomic Ingestion**: Assets download into temporary `.part` files and rename only upon completed checksum verification.
3. **Magic Byte Enforcement**: Validates file signatures (e.g. `%PDF-` header for PDF, `\x89PNG` for PNG). Malformed downloads are immediately rejected.
4. **Path Traversal Prevention**: Input filenames, hashes, and identifiers containing `/`, `\`, or `..` are rejected.
5. **Size Bounds**: Enforces maximum asset file size (default 50 MB) to prevent denial-of-service and storage exhaustion.

---

## 4. Isolated Catalog Database (`openairac-charts::catalog`)

Chart metadata is stored in `openairac_charts.sqlite`, separate from `openairac.sqlite` and `little_navmap_navigraph.sqlite`. This ensures that chart features never introduce schema breaking changes to the Little Navmap navigation database contract.
