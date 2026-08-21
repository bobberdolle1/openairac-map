# VATSIM Online Provider Specification

## 1. Provider Identity

* **Provider Name**: `VATSIM`
* **Title**: Virtual Air Traffic Simulation Network
* **Endpoints**:
  * Live Network Data API v3: `https://data.vatsim.net/v3/vatsim-data.json`
  * Events API v2: `https://events.vatsim.net/api/v2/events`
* **Cadence**: 15 seconds (Data API), 10 minutes (Events API)
* **Authentication**: None required for public read-only telemetry.

---

## 2. Ingested Data Fields

### 2.1 Pilots
* `cid` (Member ID)
* `callsign` (Sanitized uppercase e.g. `BAW123`)
* `latitude` / `longitude` (Dec degrees)
* `altitude` (Feet MSL)
* `groundspeed` (Knots)
* `heading` (Degrees magnetic 0–359)
* `transponder` (4-digit squawk code)
* `flight_plan`:
  * `aircraft_short` / `aircraft` (ICAO type designator e.g. `B789`)
  * `departure` / `arrival` / `alternate` (4-letter ICAO)
  * `flight_rules` (`I` IFR / `V` VFR)
  * `altitude` (Cruising level e.g. `FL350`)
  * `route` (Filed route string)
  * `remarks` (Flight plan remarks)
* `logon_time` / `last_updated` (ISO 8601 / RFC 3339 timestamps)

### 2.2 Controllers
* `cid`
* `callsign` (e.g. `KJFK_TWR`, `NY_APP`, `LON_CTR`)
* `frequency` (e.g. `119.100`)
* `facility` (Numeric 1=DEL, 2=GND, 3=TWR, 4=APP, 5=CTR, 6=FSS)
* `rating` (Controller rating S1–I3)
* `visual_range` (Visual range NM)
* `text_atis` (Controller ATIS / controller info lines)

### 2.3 ATIS
* `callsign` (e.g. `KJFK_ATIS`, `EGLL_A_ATIS`)
* `frequency` (e.g. `128.725`)
* `atis_code` (Single letter e.g. `B`, `C`)
* `text_atis` (Raw broadcast text lines)
* `airport_ident` (Extracted 4-letter ICAO)

---

## 3. CLI Usage Examples

```bash
# Provider listing
openairac online providers

# Live network status summary
openairac online vatsim status
openairac online vatsim status --json

# List active pilots
openairac online vatsim pilots --limit 20
openairac online vatsim pilots --callsign BAW

# List active ATC controllers
openairac online vatsim controllers

# Inspect airport online status (ATC, ATIS, traffic)
openairac online vatsim airport KJFK
openairac online vatsim airport EGLL

# Inspect decoded ATIS
openairac online vatsim atis KJFK

# Analyze route online awareness
openairac online vatsim route KJFK LFPG

# List active and upcoming events
openairac online vatsim events
```
