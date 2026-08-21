# OpenAIRAC Online Architecture Specification (v1.8.0)

## 1. Overview and Design Principles

OpenAIRAC provides an isolated, read-only **Online Flight Network Layer** (`openairac-online`) that ingests live online simulation networks (e.g. VATSIM) without altering or contaminating authoritative canonical AIRAC navigation data.

### Inviolable Invariants:
1. **Never Invent Navigation Fixes from Flight Plans**: Online pilot filed routes (`route`) are untrusted strings. Unresolved waypoint tokens are classified as `Unresolved` and are never added to the canonical navigation database.
2. **Never Invent Controlled Airspace from Callsigns**: ATC controller callsigns (e.g. `LON_CTR`, `NY_CTR`) are enroute station identifiers; they do not construct synthetic polygon sector boundaries.
3. **Honest Association Confidence**: Route ATC associations are strictly classified into `Exact` (airport station), `KnownFacility` (approach radar), `Likely` (enroute center corridor match), or `Unresolved`.
4. **Zero Persistent Identity Tracking**: Real-time user positions, CIDs, and remarks are held exclusively in ephemeral operational caches (`openairac_online.sqlite`) with bounded retention. No long-term personal history is recorded.
5. **Fail-Closed Security & Sanitization**: All external strings, ATIS broadcasts, and event descriptions are escaped against script injection and HTML tampering.

---

## 2. Core Architecture & Types

```
                    ┌──────────────────────────────┐
                    │     Official VATSIM APIs     │
                    │  Data API v3 / Events API v2 │
                    └──────────────┬───────────────┘
                                   │ HTTPS / JSON (15s cadence)
                                   ▼
                    ┌──────────────────────────────┐
                    │    OnlineNetworkProvider     │
                    │      (VatsimProvider)        │
                    └──────────────┬───────────────┘
                                   │ Parsed & Sanitized
                                   ▼
                    ┌──────────────────────────────┐
                    │       NetworkSnapshot        │
                    │  (Pilots, ATC, ATIS, Events) │
                    └───────┬──────────────┬───────┘
                            │              │
             ┌──────────────┴──┐        ┌──┴────────────────┐
             │ Ephemeral Cache │        │ Route Awareness   │
             │ (SQLite / Mem)  │        │   & ATC Engine    │
             └─────────────────┘        └───────────────────┘
```

### 2.1 Domain Entities (`openairac_online::model`)
* **`OnlinePilot`**: CID, callsign, latitude, longitude, altitude (ft), groundspeed (kt), heading (°), aircraft type, departure/arrival ICAO, flight rules, filed route, planned altitude/TAS, remarks, logon timestamp, update timestamp.
* **`OnlineController`**: CID, callsign, frequency (MHz), facility type (`DEL`, `GND`, `TWR`, `APP`, `CTR`, `FSS`), rating, visual range (NM), ATIS text lines, associated airport, enroute flag.
* **`OnlineAtis`**: CID, callsign, frequency, ATIS letter code (`A`–`Z`), raw text lines, airport ICAO, update timestamp.
* **`OnlineEvent`**: Event ID, name, event type, UTC start/end timestamps, participating airport ICAOs, route corridors, web link, description.
* **`NetworkSnapshot`**: Generated timestamp, received timestamp, freshness classification (`Live`, `Delayed`, `Stale`, `Offline`), connected client count, collections of active pilots, controllers, ATIS, servers, prefiles, and events.

---

## 3. Freshness Classification & Extrapolation Bounds

| State | Snapshot Age | UI Display | Client Behavior |
| :--- | :---: | :---: | :--- |
| **`Live`** | $\le 35\text{ s}$ | `VATSIM LIVE` (green) | Display smooth interpolation between update ticks. |
| **`Delayed`** | $35\text{ s} < t \le 90\text{ s}$ | `VATSIM DELAYED` (amber) | Clamp extrapolation to $1.25\times$ span. |
| **`Stale`** | $90\text{ s} < t \le 300\text{ s}$ | `VATSIM STALE` (orange) | Freeze motion extrapolation; display warning. |
| **`Offline`** | $> 300\text{ s}$ or error | `VATSIM OFFLINE` (red) | Hide dynamic traffic; keep cached operational summary. |

---

## 4. Route Relevance & Spatial Awareness Engine

Given an active flight plan route ($P_1, P_2, \dots, P_n$):
1. **Corridor Traffic**: Pilots located within Great-Circle cross-track distance $d \le W_{\text{half}}$ (default 50 NM).
2. **Destination Vicinity**: Pilots located within 100 NM of destination airport.
3. **ATC Along Route**:
   * *Departure Phase*: Delivery, Ground, Tower, and Terminal Radar Approach serving departure airport.
   * *Enroute Phase*: Area Control Centers (ARTCC / ACC) associated with traversed flight information regions.
   * *Arrival Phase*: Terminal Radar Approach, Tower, Ground, and Delivery serving destination airport.
4. **Event Correlator**: Active or upcoming events matching departure, destination, or route segments.

---

## 5. Security, Sanitization & Limits

* **Coordinate Validation**: Latitude clamped to $[-90.0, 90.0]$, Longitude to $[-180.0, 180.0]$. NaN and Infinite values rejected fail-closed.
* **Altitude Limits**: Clamped to $[-2000, 100000]\text{ ft}$.
* **HTML Entity Escaping**: `escape_html()` neutralizes `<`, `>`, `&`, `"`, `'` across all user remarks and ATIS lines.
* **Length Bounds**: Callsign max 16 chars, route max 2048 chars, remarks max 2048 chars.
