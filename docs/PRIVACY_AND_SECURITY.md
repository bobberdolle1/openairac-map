# 🔒 Privacy & Security Policy

OpenAIRAC is committed to complete user privacy, transparency, and data isolation.

---

## 🛡️ Core Privacy Principles

1. **Zero Tracking & Zero Telemetry**: OpenAIRAC does not collect, log, track, or transmit any user analytics, telemetry, hardware IDs, or personal behavior metrics.
2. **No Accounts or Passwords**: OpenAIRAC requires no login, user account, email address, or registration.
3. **Local Data Isolation**: All created flight plans, aircraft performance profiles, user settings, logbooks, and imported Local AIP Vault packages remain strictly on your local computer.

---

## 🌐 Network Services Contacted

OpenAIRAC Map contacts external online services exclusively to fetch requested simulation data:

| External Service | Data Requested | When Requested | User Configurable |
|---|---|---|---|
| **NOAA AviationWeather.gov** | Real-time METAR observations, TAF forecasts, international SIGMET polygons, and PIREPs | Periodic polling during flight (default: every 15s) | Yes (can be disabled in Options) |
| **VATSIM Data API v3** | Connected pilot positions, active ATC controller stations, ATIS text | When VATSIM is selected in Online Flying | Yes (Options → Online Flying) |
| **IVAO Data Feed** | Connected pilot positions, active ATC controller stations | When IVAO is selected in Online Flying | Yes (Options → Online Flying) |
| **FAA d-TPP & France SIA** | PDF approach plates and airport diagrams | Only on-demand when opening an airport plate | Cached locally in content-addressed store |
| **OpenStreetMap / Map Servers** | Map background raster tiles | When panning or zooming the map display | Selectable offline/online map themes |
| **GitHub Releases API** | Latest tag name and release notes URL | When selecting *Check for Updates* (or periodic check) | Yes |

---

## 🔐 AI Crew Gateway Security

* The **AI Crew Gateway** binds strictly to **`127.0.0.1:8989`** (localhost loopback).
* External, remote, or LAN network connections are rejected by default.
* The API is read-only and cannot execute shell commands or modify system files.

---

## 🗄️ Local AIP Vault Security

* Datasets imported into the **Local AIP Vault** (e.g. Russian CAICA, German DFS, Eurocontrol EAD) remain in your local app data folder (`%APPDATA%\ABarthel` or `~/.config/ABarthel`).
* Local AIP Vault files are **never uploaded** to OpenAIRAC servers, GitHub, or any remote repository.
* When reporting issues or sharing log files, sensitive paths and proprietary documents are automatically scrubbed.
