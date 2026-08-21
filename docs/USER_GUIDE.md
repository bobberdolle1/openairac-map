# 📖 OpenAIRAC User Guide

Welcome to **OpenAIRAC**, the open aeronautical navigation data infrastructure and desktop flight planning suite for flight simulation.

---

> ⚠️ **FOR FLIGHT SIMULATION ONLY — NEVER USE FOR REAL-WORLD AVIATION.**  
> OpenAIRAC and OpenAIRAC Map are designed exclusively for flight simulation (X-Plane 12/11, MSFS 2024/2020). They are not certified and must never be used for real flight planning or aircraft navigation.

---

## 🧭 Overview & Quick Links

OpenAIRAC consists of two integrated components:
1. **OpenAIRAC Core**: The backend navigation data engine, temporal SQLite database, routing solver, and AI Crew Gateway.
2. **OpenAIRAC Map**: The desktop flight planner, moving map, and Electronic Flight Bag (EFB) application for Windows, macOS, and Linux.

### User Guides Index

* [🚀 **First Flight Tutorial**](FIRST_FLIGHT_TUTORIAL.md) — Step-by-step beginner guide to creating and flying your first route with public data.
* [📥 **Installation & Updates**](INSTALLATION.md) — How to install, update, and uninstall OpenAIRAC Map.
* [🎮 **Simulator Setup Guide**](SIMULATOR_SETUP.md) — Connecting OpenAIRAC Map to X-Plane 12/11 and MSFS 2024/2020.
* [🗄️ **Data & Providers Guide**](DATA_AND_PROVIDERS.md) — How AIRAC cycles work, public baseline data, and the Local AIP Vault.
* [🇷🇺 **Russia / CAICA Guide**](RUSSIA_CAICA_GUIDE.md) — How to import lawful, user-supplied Russian AIP packages.
* [💾 **Saving & Exporting**](SAVING_AND_EXPORTING.md) — Exporting flight plans to X-Plane, MSFS, PMDG, and Garmin formats.
* [🤖 **AI Crew Gateway**](AI_CREW_GATEWAY.md) — Using the localhost REST API with FlightdeckOS and companion apps.
* [🔧 **Troubleshooting Guide**](TROUBLESHOOTING.md) — Solving common connection, display, and permission issues.
* [❓ **Frequently Asked Questions (FAQ)**](FAQ.md) — Answers to common user questions.
* [🔒 **Privacy & Security**](PRIVACY_AND_SECURITY.md) — Information on network services, telemetry, and local data protection.

---

## ⚡ Main Screen Layout

When you launch OpenAIRAC Map, you will see a clean multi-panel interface:

```text
┌────────────────────────────────────────────────────────────────────────┐
│ Menu Bar: File • Edit • View • Flight Plan • Map • OpenAIRAC • Tools • Help
├───────────────────┬────────────────────────────────────────────────────┤
│ Docks (Left/Tabs) │                      Main Map                      │
│                   │                                                    │
│ [Flight Planning] │  Interactive OpenStreetMap / Marble Virtual Globe  │
│  - Origin / Dest  │  - Shows airports, navaids, fixes, and MORA grid   │
│  - Altitude / Spd │  - Visual flight plan route with waypoints         │
│  - SIDs & STARs   │  - Live VATSIM / IVAO traffic & ATC sectors        │
│                   │  - Real-time aircraft position & track vector      │
│ [Airport / Docks] │                                                    │
│  - Weather METAR  ├────────────────────────────────────────────────────┤
│  - Charts Viewer  │                 Elevation Profile                  │
│  - Active Flight  │  - Top of Climb (TOC) & Top of Descent (TOD)       │
│  - System Diag    │  - Vertical profile & airspace altitude boundaries │
└───────────────────┴────────────────────────────────────────────────────┘
```

---

## 🛡️ Navigation Data Policy

OpenAIRAC delivers full aeronautical coverage without commercial paywalls:
* **Bundled Public Baseline**: Includes US FAA CIFP, OurAirports, OpenFlightmaps, and France SIA datasets out of the box.
* **No Proprietary Paywalls**: OpenAIRAC **NEVER** redistributes proprietary datasets (Navigraph, Jeppesen, NavDataPro).
* **Local AIP Vault**: For official national AIP datasets that do not grant public redistribution rights (e.g. Russian CAICA), users can import their legally acquired files locally for personal simulation use.

---

## 🆘 Getting Help

* **In-App Help**: Access guides and tutorials directly via the **OpenAIRAC** or **Help** menus.
* **Log Files**: Open your active log folder anytime via **OpenAIRAC → Open Log Folder**.
* **Issue Tracker**: Report bugs and feature requests at [GitHub Issues](https://github.com/bobberdolle1/openairac-map/issues).
