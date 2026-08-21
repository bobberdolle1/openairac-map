# OpenAIRAC Online Data & Privacy Policy

## 1. Public API and Data Consumer Contract

OpenAIRAC is an independent, open-source aviation platform. The online network module acts strictly as a **read-only observer and data consumer** of official public network telemetry (e.g. VATSIM Data API v3).

### Boundaries:
1. **No Aircraft Connection / Position Broadcast**: OpenAIRAC does not connect users as active online pilots, transmit position packets, voice audio, or squawk codes onto simulation networks.
2. **No User Account Requirement**: All online awareness features operate seamlessly using public network telemetry without requiring user accounts or passwords.
3. **No Password Handling**: OpenAIRAC never collects, prompts for, or stores network passwords.
4. **Privacy-Aware Presentation**: Callsigns, aircraft types, and flight levels are the primary map identifiers. Real user names are not mass-indexed or used as map labels.
5. **Ephemeral Storage**: Online network data is stored in short-lived operational caches (`openairac_online.sqlite`) with bounded retention. Historical pilot positions are never permanently retained.

---

## 2. Navdata Integrity Boundary

Online simulation feeds are treated as untrusted external operational telemetry:
* Waypoints in filed route strings are never imported into canonical AIRAC navigation databases.
* Callsigns and transceiver positions never construct synthetic controlled airspace polygons.
* The integrity and temporal cadence of official AIRAC navigation cycles remain 100% authoritative and uncontaminated.
