# OpenAIRAC Chart Providers

This document details the official aeronautical chart providers supported by OpenAIRAC.

---

## 1. Supported Chart Providers

| Provider ID | Authority | Jurisdiction | Format | License Policy | Distribution |
|---|---|---|---|---|---|
| **`FAA_DTPP`** | Federal Aviation Administration (FAA) | United States | XML Catalog (`d-TPP_Metafile.xml`) + PDF | Public Domain (U.S. Govt) | `PublicRedistribution` |
| **`FR_SIA`** | Service de l'Information Aéronautique (DGAC) | France | eAIP AD 2.24 Catalog + PDF | Licence Ouverte v2.0 (Etalab) | `PublicRedistribution` / `LocalOnly` |

---

## 2. FAA d-TPP (Digital - Terminal Procedures Publication)

- **Source**: Official FAA Aeronautical Information Services portal (`https://aeronav.faa.gov/d-tpp/`).
- **Update Cadence**: 28-day AIRAC cycle.
- **Catalog Protocol**: Parses official machine-readable `d-TPP_Metafile.xml`.
- **Download Workflow**: On-demand lazy fetching of individual PDF plates via `https://aeronav.faa.gov/d-tpp/<cycle>/<pdf_name>`.
- **Chart Categories**:
  - `APD`: Airport Diagrams (e.g. `00610AD.PDF`)
  - `IAP`: Standard Instrument Approach Procedures (ILS, RNAV, VOR, NDB, LOC)
  - `DP`: Departure Procedures / SIDs (e.g. `00610SKORR.PDF`)
  - `STAR`: Standard Terminal Arrival Routes (e.g. `00610CAMRN.PDF`)
  - `MIN`: Takeoff and Alternate Minima (e.g. `NE2TO.PDF`)
  - `HOT`: Hot Spot Warning Pages

---

## 3. France SIA eAIP

- **Source**: Direction Générale de l'Aviation Civile / Service de l'Information Aéronautique (`https://www.sia.aviation-civile.gouv.fr/`).
- **Update Cadence**: 28-day AIRAC cycle.
- **Catalog Protocol**: Structured index of Section AD 2.24 charts for French aerodromes (`LFPG`, `LFPO`, `LFMN`, `LFLL`, `LFBO`).
- **Chart Categories**:
  - `ADC`: Aerodrome Chart - ICAO
  - `APDC`: Aircraft Parking and Docking Chart
  - `GMC`: Ground Movement Chart
  - `SID`: Standard Departure Charts (e.g. `LFPG_SID_08.PDF`)
  - `STAR`: Standard Arrival Charts (e.g. `LFPG_STAR_ALL.PDF`)
  - `IAC`: Instrument Approach Charts (ILS, RNP, VOR)
  - `VAC`: Visual Approach Charts
- **Truthful Absence Guarantee**: For `LFPG`, charts are fully indexed and viewable, while machine-readable navigation procedures remain absent (0) to match the real public open AIXM dataset truth.
