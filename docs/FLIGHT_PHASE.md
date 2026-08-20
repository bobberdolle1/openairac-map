# OpenAIRAC Map — Flight Phase Automation Engine

## 1. Phase State Machine

The `FlightPhaseEngine` evaluates aircraft telemetry and advances through deterministic flight phases:

```text
  [Preflight] ──(GS > 3 kt)──> [TaxiOut] ──(GS > 45 kt)──> [Takeoff]
                                                              │
                                                        (Airborne)
                                                              ▼
                                                        [InitialClimb]
                                                              │
  [Cruise] <──(Level Flight)── [Climb] <──(SID / > 30NM)── [Departure]
     │
 (Descend)
     ▼
  [Descent] ──(STAR / < 60NM)──> [Arrival] ──(IAP / < 15NM)──> [Approach]
                                                                  │
                                                           (Final Segment)
                                                                  ▼
  [Parked] <──(GS < 3 kt)── [TaxiIn] <──(Rollout)── [Landing] <── [Final]
```

---

## 2. Robustness Guarantees

1. **Hysteresis**: Prevents rapid oscillating transitions during turbulent flight.
2. **Slew & Teleport Detection**: Detects sudden impossible altitude jumps ($> 10,000\text{ ft}$ in $< 5\text{ s}$) and safely resets the phase without skipping intermediate logic.
3. **Transparent Evidence**: Every phase decision includes human-readable evidence (e.g. `Airborne, climbing rapidly (VS 2200 fpm, AGL 1200 ft)`).
