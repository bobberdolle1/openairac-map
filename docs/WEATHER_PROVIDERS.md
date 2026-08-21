# OpenAIRAC Weather Providers

This document details the official weather providers and endpoints supported by OpenAIRAC.

---

## 1. Supported Weather Providers

| Provider ID | Authority | Coverage | Format | Status |
|---|---|---|---|---|
| **`NOAA_AWC`** | NOAA Aviation Weather Center (`aviationweather.gov`) | Worldwide | JSON / GeoJSON Data API | Production / Authoritative |
| **`NOAA_NCEP_GFS`** | NOAA / NCEP NOMADS | Global | GRIB2 / Filter API (0.25° Grid) | Model Forecast |
| **`SIMULATOR`** | Simulator Engine (X-Plane / MSFS) | Local Simulated Environment | Native Telemetry | Simulated Local Weather |

---

## 2. NOAA AviationWeather.gov Data API

- **Base URL**: `https://aviationweather.gov/api/data`
- **User-Agent**: `OpenAIRAC/1.7 (open aviation weather client)`
- **Endpoints**:
  - `GET /metar?ids=KJFK,LFPG&format=json`: Live surface observations worldwide.
  - `GET /taf?ids=KJFK,LFPG&format=json`: Terminal aerodrome forecasts with forecast periods.
  - `GET /isigmet?format=geojson`: Active International SIGMET hazard polygons.
  - `GET /airsigmet?format=geojson`: US Domestic AIRMET/SIGMET polygons.
  - `GET /pirep?id=KJFK&distance=150&format=json`: In-flight pilot reports.

---

## 3. Rate Limiting & Network Resilience

1. **Debounce Guards**: Client requests for the same airport station are debounced (minimum 15 seconds between network dispatches).
2. **Offline Fallback**: When network access is unavailable, the local cache provides the last known observation with an explicit `OFFLINE CACHE` badge and age indicator.
3. **MIME & JSON Hardening**: Responses are strictly validated against JSON schema and GeoJSON specifications.
