# OpenAIRAC Data Provider Precedence & Conflict Resolution

## 1. Resolution Hierarchy

When composing multi-source aeronautical data, OpenAIRAC applies strict domain-specific precedence rules:

| Entity Type | Hierarchy Tier 1 (Highest) | Hierarchy Tier 2 | Hierarchy Tier 3 (Fallback) |
|---|---|---|---|
| **Airports / Aerodromes** | Official National CAA (FAA, SIA, DFS) | Verified Open Geodata (INSPIRE) | OurAirports (Global Baseline) |
| **Runways** | Official CAA Threshold Coordinates | Verified Geodesic Calculations | OurAirports Runway Records |
| **Radio Navigation Aids** | Official CAA Frequency/Coordinates | Official Open Geodata | OurAirports Navaids |
| **Enroute Fixes & Airways** | Official National AIRAC Publication | Regional Open Aeronautical Data | None (Never Fabricated) |
| **Terminal Procedures** | Official CAA Publication (FAA, SIA) | Atomic Provider Definition | None (Never Invented) |

## 2. Integrity Constraints
1. **Never Merge Legs**: Never splice legs from different providers into a single procedure.
2. **Never Overwrite Authoritative Geometry**: Community data cannot overwrite official threshold coordinates.
3. **Preserve Identity Namespaces**: Duplicate identifiers in different FIRs/countries are kept distinct via provider namespacing (`faa:K2:KJFK`, `sia:LFPG`, `ourairports:EGLL`).
