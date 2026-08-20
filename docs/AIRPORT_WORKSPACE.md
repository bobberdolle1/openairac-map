# OpenAIRAC Map — Unified Airport Workspace

## 1. Panels and Capabilities

The Unified Airport Workspace (`openairac::AirportWorkspace`) consolidates all information about an airport into a single docked panel:

### 1. Overview
* ICAO identifier, airport elevation, and country.
* Badges: `[OA] Navdata`, `[WX] VFR/IFR`, `[CHARTS] Available Plates`, `[VATSIM] Active ATC`.
* Summary of current surface weather and active online ATC.

### 2. Weather
* Decoded live METAR surface observation.
* Terminal Aerodrome Forecast (TAF) with forecast at estimated arrival time.

### 3. Charts
* Published government chart plates (FAA d-TPP / France SIA eAIP).
* Categorized by Airport Diagram, Departure, Arrival, Approach, and General.

### 4. Procedures
* Canonical machine-readable SIDs, STARs, and Instrument Approaches.
* Truthful reporting: airports with 0 public machine-readable procedures (e.g. LFPG in public SIA data) clearly state chart availability vs navdata absence.

### 5. Online (VATSIM)
* Active delivery, ground, tower, and approach controllers with frequencies.
* Current ATIS phonetic letter and raw broadcast text.
* Real-time filed arrival and departure traffic counts.

### 6. Runways & Wind
* Runway dimensions, surface type, magnetic heading, and ILS/LOC frequencies.
* **Live Wind Components**: Calculated headwind/tailwind and crosswind components based on current METAR wind.

### 7. Ground Map (SIM)
* Situational awareness airport moving map showing runways, taxiways, aprons, gates, and live ownship.
