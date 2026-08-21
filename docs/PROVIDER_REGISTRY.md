# OpenAIRAC Provider Registry Documentation

The OpenAIRAC Provider Registry is maintained in machine-readable format in `data/providers.yaml` and compiled into the `openairac-model` crate.

## Registered Providers

### 1. `FAA_CIFP`
- **Authority**: Federal Aviation Administration (United States)
- **Format**: ARINC 424-18 fixed-width records
- **License**: Public Domain (US Government Work)
- **Redistribution**: `public_redistribution`
- **Cadence**: 28-day AIRAC
- **Coverage**: Airports, Runways, Navaids, Waypoints, Airways, SIDs, STARs, Approaches, LPV FAS, MSA, MORA.
- **Scope**: United States national airspace and territories.

### 2. `FAA_AIXM`
- **Authority**: Federal Aviation Administration (United States)
- **Format**: AIXM 5.1 XML/GML
- **License**: Public Domain (US Government Work)
- **Redistribution**: `public_redistribution`
- **Cadence**: 28-day AIRAC
- **Coverage**: Airports, Runways, Navaids, Waypoints, Airways, SIDs, STARs, Approaches.

### 3. `OurAirports`
- **Authority**: OurAirports Open Data Community
- **Format**: CSV Tables
- **License**: CC0 1.0 (Public Domain Dedication)
- **Redistribution**: `public_redistribution`
- **Cadence**: Continuous daily snapshots
- **Coverage**: Airports, Runways, Radio Navaids (Worldwide).

### 4. `OpenFlightmaps`
- **Authority**: open flightmaps association (OFMA)
- **Format**: AIXM 5.x / OFMX
- **License**: OFMA Open Data
- **Redistribution**: `public_redistribution`
- **Cadence**: 28-day AIRAC
- **Coverage**: Airports, Runways, Navaids, Waypoints, Airway Segments (European airspace).

### 5. `DFS_Germany`
- **Authority**: DFS Deutsche Flugsicherung GmbH
- **Format**: AIXM 5.1 XML
- **License**: GeoNutzV Open Data
- **Redistribution**: `public_redistribution`
- **Cadence**: 28-day AIRAC
- **Coverage**: German AIP navigation data, SIDs, STARs, Approaches, Navaids, Runways.

### 6. `Eurocontrol_EAD`
- **Authority**: EUROCONTROL European AIS Database
- **Format**: AIXM 5.1 / AIXM 4.5
- **License**: Eurocontrol EAD Terms of Use (Authenticated Personal User Access)
- **Redistribution**: `local_only`
- **Cadence**: 28-day AIRAC
- **Coverage**: European AIP datasets (Airports, Runways, Navaids, Procedures).
- **Policy**: Allowed for personal local compilation; excluded from public bundle publication.

### 7. `BYOD_AIXM`
- **Authority**: User Bring-Your-Own-Data
- **Format**: AIXM 5.x XML
- **License**: BYOD Local User License
- **Redistribution**: `local_only`
- **Policy**: Imported directly via `openairac import aixm <file>`. Excluded from public release distribution.

### 8. `BYOD_ARINC424`
- **Authority**: User Bring-Your-Own-Data
- **Format**: ARINC 424-18/19/20/21
- **License**: BYOD Local User License
- **Redistribution**: `local_only`

### 9. `Navigraph_Forbidden`
- **Authority**: Navigraph / Jeppesen
- **Format**: Proprietary
- **License**: Proprietary-Restricted
- **Redistribution**: `forbidden`
- **Policy**: Strictly rejected and blocked.
