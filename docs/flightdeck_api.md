# OpenAIRAC 3.2 — FlightdeckOS & AI Crew Gateway Documentation

## Architecture & Overview

OpenAIRAC serves as the deterministic aviation and navigation backend for an external AI flight crew or **FlightdeckOS** consumer.

- **AI reasoning stays above OpenAIRAC.**
- **Navigation truth stays inside OpenAIRAC.**

No internal LLMs are run inside the OpenAIRAC Core engine. OpenAIRAC deterministically ingests raw simulator telemetry, tracks geodesic active legs, computes cross-track deviation (`XTK`), infers flight phases with multi-tick hysteresis, estimates Top-of-Descent (`TOD`), evaluates rule-based crew advisories, and formats structured briefings.

```
┌─────────────────────────────────────────────────────────────┐
│              AI Flight Crew / FlightdeckOS                  │
│       (Narration, Reasoning, Checklists, Assistance)        │
└──────────────────────────────▲──────────────────────────────┘
                               │
               REST / JSON API (localhost:8989)
               - /flightdeck/snapshot
               - /flightdeck/compact
               - /flightdeck/events
               - /flightdeck/advisories
               - /flightdeck/arrival-brief
                               │
┌──────────────────────────────┴──────────────────────────────┐
│              OpenAIRAC Map / Core Engine                    │
│   FlightExecutionSession + CrewAdvisoryEngine + Events      │
└──────────────────────────────▲──────────────────────────────┘
                               │ UDP / SimConnect
┌──────────────────────────────┴──────────────────────────────┐
│               Flight Simulator (X-Plane / MSFS)             │
└─────────────────────────────────────────────────────────────┘
```

---

## 1. Flightdeck Snapshot v2 Schema (`flightdeck_snapshot_v2`)

The primary machine-readable point-in-time state interface:

| Field | Type | Description |
| :--- | :--- | :--- |
| `schema_version` | String | `"flightdeck_snapshot_v2"` |
| `session_id` | String | Unique execution session ID |
| `timestamp` | RFC3339 | Instant of snapshot generation |
| `simulator` | String | Ingestion source identifier (e.g. `"X-Plane 11/12 Protocol"`) |
| `connection_state` | Enum | `"CONNECTED"`, `"STALE"`, `"DISCONNECTED"`, `"RECONNECTING"` |
| `flight_phase` | Enum | `"PREFLIGHT"`, `"TAXI_OUT"`, `"TAKEOFF"`, `"INITIAL_CLIMB"`, `"CLIMB"`, `"CRUISE"`, `"DESCENT"`, `"APPROACH"`, `"LANDING"`, `"TAXI_IN"`, `"PARKED"`, `"GO_AROUND"` |
| `phase_evidence` | String | Deterministic evidence explaining current phase |
| `aircraft` | Object | Aircraft profile (`icao_type`, `model_name`, `cruise_altitude_ft`, `cruise_speed_kts`) |
| `origin` | Object | Departure airport brief (`ident`, `name`, `elevation_ft`, `selected_runway`, `procedure_name`, `transition_name`, `initial_or_final_restrictions`, `provider_name`) |
| `destination` | Object | Arrival airport brief (`ident`, `name`, `elevation_ft`, `selected_runway`, `procedure_name`, `transition_name`, `initial_or_final_restrictions`, `is_source_required`, `source_required_note`) |
| `alternate` | Object / null | Alternate airport brief |
| `position` | Object / null | WGS84 aircraft position (`latitude_deg`, `longitude_deg`, `altitude_msl_ft`, `groundspeed_kts`, `vertical_speed_fpm`, `track_true_deg`, `on_ground`) |
| `active_leg` | Object / null | Active leg info (`leg_index`, `leg_name`, `prev_fix`, `next_fix`, `leg_type`, `desired_track_deg`, `distance_nm`, `altitude_constraint`) |
| `next_constraint` | Object / null | Next upcoming restriction (`fix_ident`, `constraint_type`, `distance_to_constraint_nm`, `is_active`) |
| `navigation_geometry` | Object | Cross-track and along-track progress (`xtk_nm`, `xtk_side`, `is_off_route`, `distance_to_next_fix_nm`, `remaining_route_distance_nm`, `direct_destination_distance_nm`, `ete_next_fix_sec`, `ete_destination_sec`) |
| `descent_profile` | Object | Top of Descent and VNAV monitoring (`tod_distance_nm`, `required_descent_rate_fpm`, `profile_deviation_ft`, `profile_status`) |
| `weather_summary` | Object | Real-time weather (`destination_metar`, `destination_taf`, `destination_runway_wind`, `weather_stale`) |
| `online_atc` | Array | Relevant online ATC controllers (`network`, `callsign`, `frequency_mhz`, `facility_type`) |
| `advisories` | Array | Active rule-based crew advisories (`level`, `code`, `message`, `evidence`) |
| `data_provenance` | Object | Dataset provenance (`active_provider_datasets`, `airac_cycle`, `source_required_items`, `confidence`) |
| `stale_flags` | Object | Freshness flags (`telemetry_stale`, `telemetry_age_ms`, `weather_stale`, `navdata_stale`) |
| `navigation_warnings` | Array | Flight plan validation diagnostics and warnings |

---

## 2. Compact AI Snapshot Schema (`compact_ai_snapshot_v1`)

A lightweight, low-token context representation tailored for LLM crew reasoning and speech narration:

```json
{
  "schema_version": "compact_ai_snapshot_v1",
  "flight": "UUEE -> URFF",
  "phase": "CRUISE",
  "aircraft": "TU154",
  "position": "LAT 52.4100° LON 37.8900° | 36000 ft MSL | GS 460 kt | TRK 195°",
  "active_leg": "EMGAS -> BURUD (ATS_ROUTE | Desired TRK: 195°)",
  "next_fix": "BURUD (84.2 NM, ETE: 10m 59s)",
  "next_constraint": "BURUD: FL360",
  "xtk": "0.20 NM RIGHT (ON ROUTE)",
  "route_remaining": "385.4 NM (ETE: 50m 16s)",
  "tod": "42.5 NM",
  "descent_profile": "CRUISE_LEVEL (Req VS: -1850 fpm | Dev: 0 ft)",
  "arrival": "BURUD 2Y / ILS 19R / RWY 19R (URFF)",
  "destination_weather": "URFF 19012KT 9999 SCT030 22/14 Q1013",
  "online_atc": ["URFF_APP [APP] (125.700 MHz)"],
  "advisories": [],
  "provenance": "CAICA, WORLD_OPEN | AIRAC 2608",
  "freshness": "CONNECTED (telem: 150ms age)",
  "warnings": []
}
```

---

## 3. Crew Advisory Engine Rules

Advisories are deterministic and rule-based (`INFO`, `CAUTION`, `WARNING`):

1. **`TOD_APPROACHING` (CAUTION)**: In Cruise phase when distance to Top of Descent $\le 15.0\text{ NM}$.
2. **`DESCENT_REQUIRED` (WARNING)**: Past TOD in Cruise when altitude is $> 1,500\text{ ft}$ above the standard 3.0° descent profile.
3. **`OFF_ROUTE` (WARNING)**: Cross-track deviation exceeds corridor tolerance ($> 2.5\text{ NM}$ enroute or $> 1.0\text{ NM}$ in terminal area).
4. **`TELEMETRY_STALE` (WARNING)**: Telemetry packet age $> 5.0\text{ seconds}$.
5. **`SIGNIFICANT_TAILWIND` (CAUTION)**: Selected arrival runway has tailwind $> 5.0\text{ kts}$.
6. **`ALTITUDE_CONSTRAINT_APPROACHING` (INFO)**: Distance to next waypoint altitude restriction $\le 10.0\text{ NM}$.
7. **`SOURCE_REQUIRED_PROCEDURE` (CAUTION)**: Destination or origin terminal procedures require official source dataset (e.g. `URAS` Abkhazia).

---

## 4. Flightdeck Event Stream & Delta Detection

- **Ring Buffer (`FlightEventStream`)**: Bounded event stream storing monotonic event records (`id`, `timestamp`, `event_type`, `description`, `metadata`).
- **Semantic Event Types**:
  `SIM_CONNECTED`, `SIM_DISCONNECTED`, `TELEMETRY_STALE`, `FLIGHT_STARTED`, `TAXI_STARTED`, `TAKEOFF`, `PHASE_CHANGED`, `FIX_SEQUENCED`, `SID_COMPLETED`, `TOP_OF_CLIMB`, `TOD_REACHED`, `DESCENT_STARTED`, `STAR_ENTERED`, `APPROACH_ENTERED`, `RUNWAY_CHANGED`, `GO_AROUND`, `LANDING`, `FLIGHT_COMPLETED`, `OFF_ROUTE`, `ROUTE_REJOINED`, `WEATHER_CHANGED`, `ATC_CHANGED`, `ADVISORY_ISSUED`.
- **State Delta Detector (`FlightStateDeltaDetector`)**: Computes semantic differences between consecutive snapshots so consumers do not need to diff raw JSON trees.

---

## 5. Airport Multi-Identity & Source Truth

OpenAIRAC preserves provider-scoped identities and strictly prohibits procedure fabrication:

- **Sukhumi / Babushara**:
  - `URAS`: Authoritative Russian AIS / CAICA designation (domestic AIP).
  - `UGSS`: International / OurAirports / Georgian ICAO designation.
  - `SUI`: IATA code.
  - **Status**: `SOURCE_REQUIRED` (0 fabricated procedures; STAR and Approach return `null` / `SOURCE_REQUIRED`).
- **Simferopol**:
  - `URFF`: Authoritative CAICA domestic designation (`BURUD 2Y`, `ILS 19R` available).
  - `UKFF`: International Ukrainian ICAO designation.
  - `SIP`: IATA code.

---

## 6. Security, Privacy & Failure Isolation

- **Localhost by Default**: The OpenAIRAC API binds strictly to `127.0.0.1:8989`. No external access is permitted without explicit configuration.
- **Privacy First**: No simulator telemetry, user flight plans, or navdata are uploaded to third-party services.
- **Failure Isolation**: If an external AI consumer or FlightdeckOS crashes or disconnects, OpenAIRAC Map, simulator connection, and flight execution continue unaffected.
