# OpenAIRAC World Open Architecture & Federation Policy

## 1. Principle of Honest World-Open Federation
OpenAIRAC World Open combines verified official national aeronautical sources and open community datasets into a single, provenance-preserving aeronautical database.

### Core Architecture
1. **Authoritative National Overrides Community**:
   - **United States**: US FAA CIFP provides authoritative navigation data, procedures, airways, and fixes.
   - **France**: DGAC / SIA France provides official aerodromes, runways, navaids, and structured DATA procedure tables (SIDs, STARs, RNP approaches).
   - **Germany**: DFS INSPIRE provides official German open transport network geodata.
   - **Global Baseline**: OurAirports provides global public-domain fallback coverage for worldwide airports, runways, and navaids across 200+ countries.

2. **Atomic Procedures**:
   - Terminal procedures are never merged across providers leg-by-leg. A procedure is an atomic object owned completely by its authoritative provider.

3. **Strict Distribution Taint Guard**:
   - Public release bundles categorically reject all `LocalOnly` or `Forbidden` sources (Navigraph, Jeppesen, ENAIRE LocalOnly, restricted DFS AIS, user BYOD).

4. **Lifecycle Fidelity**:
   - AIRAC cycle datasets (FAA, SIA) retain 28-day cycle semantics.
   - Non-AIRAC datasets (OurAirports, DFS INSPIRE) retain continuous snapshot retrieval timestamps.
