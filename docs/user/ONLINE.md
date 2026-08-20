# Online Flying & VATSIM Network Awareness

## 1. VATSIM Live Telemetry

OpenAIRAC Map queries official VATSIM Data API v3 endpoints every 15 seconds:
* **Live Pilots**: Position, altitude, groundspeed, heading, and filed flight plans.
* **Smooth Interpolation**: Renders continuous aircraft motion between data pulses.
* **ATC Controllers**: Shows active delivery, ground, tower, approach, and enroute center frequencies and visual range.
* **Airport ATIS**: Decoded information letter and broadcast text.

---

## 2. Privacy & Security

* **Zero Credentials**: No VATSIM CID or password is required or requested.
* **Observer Mode**: OpenAIRAC Map is a read-only situational awareness tool; it does not transmit your simulator position to the network.
