# OpenAIRAC Map — Airport Moving Map Architecture

## 1. Concept and Provenance

The Airport Moving Map displays airport layout geometry (runways, taxiways, aprons, parking spots) and live simulator aircraft position for ground situational awareness.

### Provenance Separation:
* **Geometry Source**: `[SIM] Simulator Scenery` (Extracted from X-Plane / MSFS scenery databases).
* **Official Plates**: `[CHARTS] Government PDF` (FAA d-TPP / France SIA eAIP airport diagrams).

The Airport Moving Map is clearly distinguished from official published PDF plates. Standard PDF plates are not distorted or manipulated to act as synthetic moving maps unless authoritative geospatial calibration metadata is officially provided.
