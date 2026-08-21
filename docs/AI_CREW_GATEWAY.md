# 🤖 OpenAIRAC AI Crew Gateway

The **AI Crew Gateway** is a high-performance, deterministic localhost REST API integrated into OpenAIRAC Core and OpenAIRAC Map.

It allows external flight deck automation systems (such as **FlightdeckOS**), AI co-pilot agents, and companion EFB tools to query active flight status, navigation geometry, descent profiles, weather observations, and procedure constraints without screen scraping or simulator memory hacking.

---

## 🔒 Security & Architecture

* **Loopback Only**: Binds exclusively to `127.0.0.1:8989`. External or remote network connections are strictly rejected.
* **Read-Only / Deterministic**: Provides factual aeronautical data and flight context. The Gateway cannot execute arbitrary code or modify simulator state.
* **Standard REST / JSON**: Clean HTTP endpoints returning standardized JSON payloads.

---

## 📡 Gateway Endpoints

### 1. Canonical Flight Telemetry Snapshot (`/api/openairac/v1/snapshot`)
* **Method**: `GET`
* **Schema**: `OpenAiracSnapshotV2`
* **Description**: Complete multi-subsystem flight telemetry snapshot including position, active leg, next constraint, descent profile, weather summary, online ATC stations, and provider provenance.

#### Example Response:
```json
{
  "schema_version": "openairac.snapshot.v2",
  "session_id": "exec_LFPG_LFBO_1724248800",
  "timestamp": "2026-08-21T16:00:00Z",
  "simulator": "X-Plane 12 Protocol",
  "connection_state": "CONNECTED",
  "flight_phase": "CRUISE",
  "aircraft": {
    "icao_type": "A320",
    "cruise_altitude_ft": 31000,
    "cruise_speed_kts": 450
  },
  "origin": {
    "ident": "LFPG",
    "selected_runway": "27L",
    "procedure_name": "OKASI 5A",
    "sid_procedure": "OKASI 5A",
    "is_source_required": false
  },
  "destination": {
    "ident": "LFBO",
    "selected_runway": "14R",
    "procedure_name": "VELIN 6A",
    "star_procedure": "VELIN 6A",
    "approach_procedure": "ILS 14R",
    "approach_type": "ILS",
    "is_source_required": false
  },
  "position": {
    "latitude_deg": 46.1200,
    "longitude_deg": 1.8900,
    "altitude_msl_ft": 31000.0,
    "groundspeed_kts": 450.0,
    "track_true_deg": 195.0,
    "on_ground": false
  },
  "active_leg": {
    "leg_name": "OKASI -> VELIN",
    "prev_fix": "OKASI",
    "next_fix": "VELIN",
    "desired_track_deg": 195.0,
    "distance_nm": 180.0
  },
  "descent_profile": {
    "tod_distance_nm": 45.2,
    "profile_status": "CRUISE_LEVEL",
    "required_descent_rate_fpm": null
  },
  "freshness": {
    "telemetry": { "status": "CURRENT", "age_ms": 120 },
    "weather": { "status": "CURRENT", "age_sec": 45 },
    "online_atc": { "status": "CONNECTED", "network": "VATSIM" },
    "navdata": { "status": "CURRENT", "primary_provider": "France SIA" }
  }
}
```

---

### 2. Compact AI Snapshot (`/api/openairac/v1/snapshot/compact`)
* **Method**: `GET`
* **Schema**: `OpenAiracCompactSnapshot`
* **Description**: Ultra-compact token-optimized snapshot designed for direct ingestion into LLM context windows (< 150 tokens).

---

### 3. Crew Tool Definitions & Execution
* **`GET /api/openairac/v1/tools`**: Returns JSON Schema definitions for available flight tools (`get_flight_state`, `get_arrival_brief`, `get_weather`, `get_online_atc`, etc.).
* **`POST /api/openairac/v1/tools/execute`**: Executes a named factual tool deterministically and returns structured evidence and source timestamps.

---

## ✈️ FlightdeckOS Integration

FlightdeckOS connects to the OpenAIRAC AI Crew Gateway automatically to establish an AI co-pilot dialogue grounded in deterministic aeronautical truth.
