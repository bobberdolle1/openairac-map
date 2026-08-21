# OpenAIRAC KLN90B GPS Compatibility Specification

## Overview

The Bendix/King KLN90B is a classic GPS receiver widely simulated in flight simulation aircraft addons, including Project Tupolev Tu-154, vasFMC, Antonov An-24/An-26, and classic Boeing/Airbus retrofits.

OpenAIRAC provides a clean-room, independently designed `Kln90bExporter` under the MIT license, generating standard `.DAT` navigation tables directly from the canonical world database without proprietary fixtures or GPL code pollution.

## Directory and File Layout

A standard KLN90B navigation package contains the following files in the navdata directory:

```text
<kln90b_navdata_root>/
├── APT.DAT               # Airport facilities, coordinates, elevations, runways
├── NAV.DAT               # VOR, VOR-DME, NDB, DME, ILS radio navigation aids
├── WPT.DAT               # Enroute and terminal navigation fixes (5-letter waypoints)
├── AWY.DAT               # Enroute airway segments and sequence definitions
├── FAS.DAT               # Terminal procedure legs (SIDs, STARs, Approaches)
└── cycle.dat             # AIRAC cycle and effective timestamp metadata
```

## File Formats

### 1. APT.DAT

Comma-separated airport records:
```text
ICAO,Name,Latitude,Longitude,ElevationFt,Runways
```
Example:
```text
EDDF,Frankfurt Main,50.033333,8.570556,364,07C/25C
KSFO,San Francisco International,37.618800,-122.375000,13,28R/10L
```

### 2. NAV.DAT

Radio navaids formatted with identifier, frequency, and coordinates:
```text
Type,Ident,Name,Frequency,Latitude,Longitude,ElevationFt,MagVar
```
Example:
```text
VOR,FFM,FRANKFURT,114.200,50.053333,8.638056,410,2.0
NDB,CHA,CHATTER,341,50.123456,8.765432,300,2.0
DME,SFO,SAN FRANCISCO,115.800,37.619500,-122.373800,12,13.5
```

### 3. WPT.DAT

Waypoints:
```text
Ident,Latitude,Longitude,Region
```
Example:
```text
RIDSU,50.150000,8.900000,ED
DF401,50.050000,8.650000,ED
```

### 4. AWY.DAT

Airway route segments:
```text
RouteIdent,SequenceNumber,FixIdent,Latitude,Longitude,MinAltitudeFt
```
Example:
```text
Y1,10,DF401,50.050000,8.650000,4000
Y1,20,RIDSU,50.150000,8.900000,4000
```

### 5. FAS.DAT

Procedure legs:
```text
Airport,Kind,ProcedureName,SequenceNumber,PathTerminator,FixIdent,Latitude,Longitude,Alt1,Alt2,SpeedLimit
```
Example:
```text
EDDF,SID,RIDSU1A,10,CF,DF401,50.050000,8.650000,4000,0,220
EDDF,SID,RIDSU1A,20,TF,RIDSU,50.150000,8.900000,5000,0,250
```

### 6. cycle.dat

Key-value metadata:
```text
AIRAC=2608
EFFECTIVE=2026-08-13T09:01:00+00:00
GENERATOR=OpenAIRAC KLN90B Exporter v1.0
FINGERPRINT=fp-kln90b-2608-1786707660
```

## Integration, Forensic Loader Analysis & Maturity

- **Target ID**: `kln90b`
- **Support State**: `EXPERIMENTAL`
- **Forensic Loader Architecture**:
  - Open-source KLN90B GPS implementations (e.g. Project Tupolev Tu-154, vasFMC) historically ingest either PSS-format files (`PSSAPT.dat`, `PSSNAV.dat`, `PSSWPT.dat`, `PSSAWY.dat`) or compile them at first gauge initialization into internal binary index cache files (`PTT_APT.DAT`, `PTT_VOR.DAT`, `PTT_NDB.DAT`, `PTT_WPT.DAT`, `PTT_SUP.DAT`).
  - The OpenAIRAC `kln90b` exporter currently serializes clean-room standard `.DAT` tables (`APT.DAT`, `NAV.DAT`, `WPT.DAT`, `AWY.DAT`, `FAS.DAT`, `cycle.dat`).
  - Transition to `SUPPORTED` requires end-to-end binary cache rebuild validation against the unmodified Project Tupolev KLN90B gauge or vasFMC loader executable.
- **Validation Strategy**: `HashVerify` (all files verified against generated hashes before activation).
- **Transactional Installer**: Staging -> transactional swap -> atomic rollback on failure.
