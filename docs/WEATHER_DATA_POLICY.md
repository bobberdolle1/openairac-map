# OpenAIRAC Weather Data & Security Policy

This document outlines the licensing, security safeguards, and data governance policies applicable to weather services in OpenAIRAC.

---

## 1. Public Domain & Open Data Governance

- **NOAA Weather Data**: Weather data provided by the U.S. National Oceanic and Atmospheric Administration (NOAA) is a work of the United States Government and is in the public domain.
- **Redistribution & Caching**: Live observations and forecasts are cached locally on client workstations for performance and resilience.
- **Attribution**: Every weather view clearly presents source authority (`NOAA Aviation Weather Center`) and observation timestamps.

---

## 2. Security Safeguards

1. **HTML Escaping**: Raw METAR, TAF, and SIGMET text can contain arbitrary characters. All raw text is strictly HTML-escaped before rendering in GUI browsers to eliminate any risk of script or markup injection.
2. **Bounds Checking**: Numerical fields (wind direction 0–360°, speeds 0–300 kt, temperatures -100°C to +70°C, coordinates -180° to +180°) are validated before processing.
3. **Response Limits**: Network responses exceeding maximum size thresholds (5 MB) are rejected to prevent denial-of-service and memory exhaustion.
