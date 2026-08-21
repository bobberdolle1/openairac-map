# OpenAIRAC EFB Data Boundaries & Architecture

## 1. Principles of Data Isolation

OpenAIRAC preserves clean separation between independent operational layers:

```text
+-----------------------------------------------------------------------------+
| Layer                  | Authority / Source     | Mutability | Storage      |
+------------------------+------------------------+------------+--------------+
| 1. Canonical Navdata   | Official AIP / CIFP    | Read-Only  | SQLite (DB)  |
| 2. Aeronautical Charts | Government d-TPP/eAIP  | Read-Only  | Content-Addr |
| 3. Weather Telemetry   | NOAA / NWS / AWC       | Ephemeral  | SQLite Cache |
| 4. Online Traffic      | VATSIM Data API v3     | Ephemeral  | Memory/Cache |
| 5. Simulator Telemetry | Live Sim Connection    | Runtime    | In-Memory    |
| 6. EFB Flight Session  | User Flight Plan       | Session    | User Session |
+------------------------+------------------------+------------+--------------+
```

---

## 2. Inviolable Safety & Provenance Rules

1. **Charts do not create navigation procedure legs**: A published PDF plate does not synthesize ARINC 424 legs.
2. **Online network routes do not create navigation fixes**: Callsigns and filed strings never pollute canonical databases.
3. **Simulator scenery geometry is not canonical navdata**: Runway/taxiway polygons from simulator scenery provide visual context, not AIRAC truth.
4. **Deterministic Phase Automation**: Flight phase inferences (`Preflight` $\to$ `Cruise` $\to$ `Final`) rely on transparent mathematical state machines with hysteresis, never stochastic heuristics.
5. **Transparency over Invention**: If procedure data is missing (e.g. French SIA public XML having 0 terminal procedures for LFPG), OpenAIRAC honestly displays 0 procedures while presenting available official eAIP charts.
