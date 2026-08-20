# OpenAIRAC Automation HTTP API Specification (v1)

## 1. Overview
OpenAIRAC Map provides a localhost-bound automation HTTP API on port 8965 (`http://127.0.0.1:8965/api/openairac/v1/...`) designed for scriptable tooling, external flight simulation utilities, and future AI copilot agents.

## 2. API Endpoints

### Status & Health
- `GET /api/openairac/v1/status` — Application version, API version, and navdata metrics.
- `GET /api/openairac/v1/sim` — Simulator connection status.

### Airport & Navdata
- `GET /api/openairac/v1/airports/{icao}` — Airport details and chart counts.
- `GET /api/openairac/v1/navdata/coverage/{icao}` — Navdata coverage capabilities.
- `GET /api/openairac/v1/procedures/{icao}` — Terminal procedures.
- `GET /api/openairac/v1/charts/{icao}` — Aeronautical chart documents list.

### Flight Planning & Active Flight
- `GET /api/openairac/v1/flightplan` — Active route summary and distance.
- `GET /api/openairac/v1/flight/active` — Active flight phase and telemetry.
- `POST /api/openairac/v1/flightplan/random` — Aircraft-aware random flight plan generation with seed reproducibility.

### Weather & Online Networks
- `GET /api/openairac/v1/weather/{icao}` — Live METAR and TAF observations.
- `GET /api/openairac/v1/online` — Dual VATSIM + IVAO client status.
- `GET /api/openairac/v1/online/vatsim` — VATSIM network status.
- `GET /api/openairac/v1/online/ivao` — IVAO network status.
