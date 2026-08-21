# OpenAIRAC Garmin GNS430 / Navdata Compatibility Specification

## Overview

The Garmin GNS430/530 format is a widely adopted text-based aeronautical navigation database layout used by legacy X-Plane GPS units (GNS430/530, G1000 legacy fallback), classic aircraft flight management systems, and third-party simulation tools.

OpenAIRAC provides a deterministic, clean-room `Gns430Exporter` that compiles canonical navigation data directly into the GNS430 family structure without proprietary fixtures.

## Directory and File Layout

A complete GNS430 navigation package resides in `Custom Data/GNS430/navdata/` (or a standalone export directory) with the following structure:

```text
<export_root>/
├── Airports.txt          # Airport facilities, runways, and transition altitudes
├── Navaids.txt           # VOR, VOR-DME, VORTAC, NDB, and DME radio navigation facilities
├── Waypoints.txt         # Enroute and terminal waypoints (fixes)
├── ATS.txt               # High and Low enroute airways
├── cycle_info.txt        # AIRAC cycle metadata and generator fingerprint
└── Proc/                 # Per-airport terminal procedures
    ├── EDDF.txt          # SIDs, STARs, and Instrument Approaches for EDDF
    ├── KSFO.txt          # SIDs, STARs, and Instrument Approaches for KSFO
    └── ...
```

## Record Specifications

### 1. Airports.txt

Airport header records (`A`) followed by Runway records (`R`):

- **Airport Record**:
  ```text
  A,ICAO,Name,Latitude,Longitude,Elevation,TransitionAltitude,SpeedLimit,SpeedLimitAltitude
  ```
  Example:
  ```text
  A,EDDF,Frankfurt Main,50.033333,8.570556,364,18000,250,10000
  ```

- **Runway Record**:
  ```text
  R,Designator,TrueHeading,LengthFt,WidthFt,ILSEquipped,ILSFreq,ILSBearing,Lat,Lon,ElevationFt,GlideSlopeAngle,TCH,Surface
  ```
  Example:
  ```text
  R,07C,68,13123,197,0,0.0,0,50.033333,8.570556,364,3.00,50,ASPH
  ```

### 2. Navaids.txt

Radio navigation aids categorized by type:

- **VOR / VOR-DME / VORTAC (`V`)**:
  ```text
  V,Ident,Name,FrequencyMHz,Class,Latitude,Longitude,ElevationFt,SlavedVariation,MagVar
  ```
  Example:
  ```text
  V,FFM,FRANKFURT,114.200,H,50.053333,8.638056,410,2.0,2.0
  ```

- **NDB (`N`)**:
  ```text
  N,Ident,Name,FrequencyKHz,Class,Latitude,Longitude,ElevationFt,MagVar
  ```
  Example:
  ```text
  N,CHA,CHATTER,341,H,50.123456,8.765432,300,2.0
  ```

- **DME (`D`)**:
  ```text
  D,Ident,Name,FrequencyMHz,Class,Latitude,Longitude,ElevationFt,SlavedVar,MagVar
  ```

### 3. Waypoints.txt

Enroute and terminal fixes:

```text
W,Ident,Latitude,Longitude,Collocated,Region
```
Example:
```text
W,RIDSU,50.150000,8.900000,0,ED
W,DF401,50.050000,8.650000,0,ED
```

### 4. ATS.txt

Enroute airway segment connections:

```text
A,AirwayName,Sequence,StartFix,StartLat,StartLon,EndFix,EndLat,EndLon,Direction,MinAltFt,MaxAltFt
```
Example:
```text
A,Y1,10,DF401,50.050000,8.650000,RIDSU,50.150000,8.900000,N,4000,45000
```

### 5. Proc/<ICAO>.txt

Terminal instrument procedures:

- Header: `A,ICAO`
- SIDs: `S,SID_NAME,TRANSITION,RUNWAY`
- STARs: `A,STAR_NAME,TRANSITION,RUNWAY`
- Approaches: `I,APPROACH_NAME,TRANSITION,RUNWAY`
- Leg Lines:
  ```text
  PathTerminator,FixIdent,Lat,Lon,FlyOver,TurnDirection,AltitudeDescriptor,Alt1,Alt2,SpeedLimit,CourseDeg,DistanceNM
  ```
  Example:
  ```text
  CF,DF401,50.050000,8.650000,0, ,+,4000,0,220,73.0,5.2
  TF,RIDSU,50.150000,8.900000,0, ,+,5000,0,250,85.0,12.4
  ```

## Installer Contract & Maturity

- **Target ID**: `xplane-gns430`
- **Maturity**: `EXPERIMENTAL` (implemented, unit tested, and syntax verified; live in-cockpit simulated GNS430 load test required before transitioning to `SUPPORTED`).
- **Install Strategy**: `Subdirectory` (into `%XPLANE%/Custom Data/GNS430/navdata`)
- **Validation**: SHA-256 integrity verification of every generated file.
- **Transactional Rollback**: Supported automatically by `openairac-export`.
