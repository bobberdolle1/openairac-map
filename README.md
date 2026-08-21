# ✈️ OpenAIRAC Map

<div align="center">

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE.txt)
[![Version: 2.3.0](https://img.shields.io/badge/Version-2.3.0_win64-green.svg)](https://github.com/bobberdolle1/openairac-map/releases/latest)
[![OpenAIRAC Core](https://img.shields.io/badge/Core_Engine-v2.11.0-orange.svg)](https://github.com/bobberdolle1/open-airac)
[![Target: X-Plane & MSFS](https://img.shields.io/badge/Simulators-X--Plane_12_|_MSFS-blue.svg)](https://github.com/bobberdolle1/open-airac/blob/main/docs/SIMULATOR_SETUP.md)

**Open flight-simulation navigation data, flight planner, moving map, and Electronic Flight Bag (EFB).**

[📥 **Download OpenAIRAC Map for Windows (Installer)**](https://github.com/bobberdolle1/openairac-map/releases/latest/download/OpenAIRAC-Map-2.3.0-Setup.exe) • [📦 **Portable ZIP**](https://github.com/bobberdolle1/openairac-map/releases/latest/download/OpenAIRAC-Map-2.3.0-win64.zip) • [📖 **Documentation**](https://github.com/bobberdolle1/open-airac/blob/main/docs/USER_GUIDE.md)

</div>

---

> ⚠️ **FOR FLIGHT SIMULATION ONLY — NEVER USE FOR REAL-WORLD AVIATION.**  
> OpenAIRAC Map is designed exclusively for flight simulation (X-Plane 12/11, MSFS 2024/2020). It is not certified and must never be used for real flight planning or navigation.

---

## 🌟 What is OpenAIRAC Map?

**OpenAIRAC Map** is a complete, modern desktop flight planning, moving map, and Electronic Flight Bag application powered by the **OpenAIRAC** temporal aeronautical navigation data engine.

Unlike traditional flight planning tools that depend on proprietary subscription paywalls, OpenAIRAC Map delivers a full-featured navigation suite using **official, public-domain, and open-license government datasets** right out of the box.

```text
┌────────────────────────────────────────────────────────────────────────┐
│                            OpenAIRAC Map                               │
│  ┌──────────────────────┬──────────────────────┬────────────────────┐  │
│  │   Moving Map & EFB   │   Flight Planning    │  Official Charts   │  │
│  │  OpenStreetMap/Marble│  SIDs, STARs, IAPs   │  FAA d-TPP & SIA   │  │
│  └──────────┬───────────┴──────────┬───────────┴──────────┬─────────┘  │
│             │                      │                      │            │
│  ┌──────────▼──────────────────────▼──────────────────────▼─────────┐  │
│  │                     OpenAIRAC Navigation Engine                  │  │
│  │       Public Baseline Data + Optional Local AIP Vault            │  │
│  └─────────────────────────────────┬────────────────────────────────┘  │
│                                    │                                   │
│  ┌─────────────────────────────────▼────────────────────────────────┐  │
│  │         Live Feeds: NOAA Weather • VATSIM/IVAO Traffic           │  │
│  │           Simulator Link: X-Plane 12/11 • MSFS 2024/2020         │  │
│  └──────────────────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────────────────┘
```

---

## ⚡ Key Capabilities

* **Free Public Navdata Included**: Bundled worldwide baseline from **FAA CIFP**, **OurAirports**, **OpenFlightmaps**, and **France SIA** — fully functional immediately without accounts or subscriptions.
* **Official Government Charts**: On-demand viewing of official terminal approach plates, SIDs, STARs, and airport diagrams from FAA d-TPP and France SIA eAIP.
* **Real-Time Weather Telemetry**: Built-in NOAA AviationWeather.gov METAR observations, TAF forecasts, PIREPs, and international SIGMET hazard polygons.
* **Online Flying Awareness**: Real-time VATSIM and IVAO pilot traffic, active ATC stations, and airport ATIS broadcasts with smooth motion interpolation.
* **Flight Simulator Connection**: Native zero-configuration telemetry connection with **X-Plane 12**, **X-Plane 11**, **MSFS 2024**, and **MSFS 2020**.
* **Local AIP Vault**: Secure, personal import for official national AIP datasets that require local-only use (such as Russian **CAICA**).
* **Multi-Format Flight Plan Export**: Export directly to X-Plane FMS, MSFS PLN, PMDG RTE, Garmin FPL, and more with one click.
* **AI Crew Gateway**: Localhost REST API (`127.0.0.1:8989`) enabling AI co-pilots (such as FlightdeckOS) and companion EFB tools to query active flight status deterministically.

---

## 📥 Download & Installation

### Windows 64-bit (Recommended)

1. Download the [**OpenAIRAC Map Setup Installer (OpenAIRAC-Map-2.3.0-Setup.exe)**](https://github.com/bobberdolle1/openairac-map/releases/latest/download/OpenAIRAC-Map-2.3.0-Setup.exe).
2. Run the setup installer and follow the on-screen steps.
3. Launch **OpenAIRAC Map** from the Start Menu or Desktop shortcut.

### Portable Version (No Installation Required)

1. Download [**OpenAIRAC-Map-2.3.0-win64.zip**](https://github.com/bobberdolle1/openairac-map/releases/latest/download/OpenAIRAC-Map-2.3.0-win64.zip).
2. Extract the archive to any folder on your computer (e.g. `C:\FlightSim\OpenAIRAC-Map`).
3. Run `littlenavmap.exe` directly.

> 🔍 **Integrity Verification**: Checksum signatures are available in [SHA256SUMS.txt](https://github.com/bobberdolle1/openairac-map/releases/latest/download/SHA256SUMS.txt).

---

## 🛫 First Flight in 2 Minutes

1. **Launch OpenAIRAC Map**: The First-Run Wizard will confirm that your public navigation baseline is active.
2. **Select Origin & Destination**: In the **Flight Planning** tab on the left, type an origin (e.g. `LFPG` Paris Charles de Gaulle) and destination (e.g. `LFBO` Toulouse-Blagnac).
3. **Calculate Route**: Click **Calculate Flight Plan** to generate an optimized airway route with assigned SIDs and STARs.
4. **View Weather & Charts**: Open the **Charts** dock or **Flight Briefing** to inspect departure/arrival METAR and approach plates.
5. **Connect to Simulator**: In the menu, go to **Tools → Connect to Flight Simulator** and launch your simulator.
6. **Fly with Real-Time Map**: Track your aircraft position, groundspeed, altitude profile, and route progress in real-time.

For full step-by-step instructions, see the [**First Flight Tutorial**](https://github.com/bobberdolle1/open-airac/blob/main/docs/FIRST_FLIGHT_TUTORIAL.md).

---

## 🛡️ Navigation Data & Distribution Policy

OpenAIRAC strictly enforces copyright and data provenance:

| Data Type | Status | Source |
|---|---|---|
| **Public Baseline** | **BUNDLED & ACTIVE** | FAA CIFP (US), OurAirports (Worldwide), OpenFlightmaps (Europe), France SIA |
| **Official Government Charts** | **ON-DEMAND STREAM** | FAA d-TPP, France SIA eAIP (cached locally) |
| **Real-Time Weather & Traffic** | **LIVE ZERO-LOGIN** | NOAA AviationWeather.gov, VATSIM API v3, IVAO Data |
| **Local National AIPs (e.g. CAICA)**| **OPTIONAL BYOD** | User-supplied lawful local files imported into Local AIP Vault |
| **Proprietary Paywalls** | **STRICTLY NOT BUNDLED** | Navigraph, Jeppesen, NavDataPro are **NEVER** distributed by OpenAIRAC |

---

## 📚 Documentation & User Guides

* [📖 **Master User Guide**](https://github.com/bobberdolle1/open-airac/blob/main/docs/USER_GUIDE.md) — Complete user manual and navigation reference.
* [🚀 **First Flight Tutorial**](https://github.com/bobberdolle1/open-airac/blob/main/docs/FIRST_FLIGHT_TUTORIAL.md) — Beginner flight planning walk-through.
* [🎮 **Simulator Setup Guide**](https://github.com/bobberdolle1/open-airac/blob/main/docs/SIMULATOR_SETUP.md) — Connecting X-Plane 12/11 and MSFS 2024/2020.
* [🗄️ **Data & Providers Guide**](https://github.com/bobberdolle1/open-airac/blob/main/docs/DATA_AND_PROVIDERS.md) — Understanding AIRAC cycles, public baseline, and Local AIP Vault.
* [🇷🇺 **Russia / CAICA Guide**](https://github.com/bobberdolle1/open-airac/blob/main/docs/RUSSIA_CAICA_GUIDE.md) — Optional guide for importing Russian AIP packages.
* [💾 **Saving & Exporting**](https://github.com/bobberdolle1/open-airac/blob/main/docs/SAVING_AND_EXPORTING.md) — Multi-format flight plan exports.
* [🤖 **AI Crew Gateway**](https://github.com/bobberdolle1/open-airac/blob/main/docs/AI_CREW_GATEWAY.md) — REST API for FlightdeckOS and EFB companion apps.
* [🔧 **Troubleshooting & FAQ**](https://github.com/bobberdolle1/open-airac/blob/main/docs/TROUBLESHOOTING.md) — Solutions for common connection and setup issues.
* [🔒 **Privacy & Security**](https://github.com/bobberdolle1/open-airac/blob/main/docs/PRIVACY_AND_SECURITY.md) — Network endpoints and local data privacy.

---

## 🛠️ Building from Source (Developers Only)

Normal users do **not** need to build from source — use the [pre-built installer](https://github.com/bobberdolle1/openairac-map/releases/latest).

### Prerequisites
* Windows 10/11 64-bit, macOS 13+, or Linux (Ubuntu 24.04+)
* Qt 6.5+ (Qt 6.8.2 recommended) with `svg`, `sql`, `printsupport`, `core5compat`
* C++20 compatible compiler (GCC 13+, Clang 16+, MSVC 2022)
* CMake 3.25+ and Ninja / Make

### Build Steps
```bash
git clone https://github.com/bobberdolle1/openairac-map.git
cd openairac-map
# Follow BUILD.txt for marble and atools setup
qmake6 littlenavmap.pro CONFIG+=release
make -j$(nproc)
```

---

## 📜 Credits & Upstream Attribution

OpenAIRAC Map is distributed under the **GNU General Public License v3.0 (GPLv3)**.

OpenAIRAC Map is proudly derived from **Little Navmap** created by **Alexander Barthel** and open-source contributors. We express our deepest gratitude to Alexander Barthel and the Little Navmap community for building an extraordinary foundation for flight simulation navigation.

* **Upstream Project**: [Little Navmap by Alexander Barthel](https://github.com/albar965/littlenavmap) ([littlenavmap.org](https://www.littlenavmap.org/))
* **Virtual Globe**: [KDE Marble Project](https://marble.kde.org/)
* **UI Framework**: [The Qt Company](https://www.qt.io/)
* **Aeronautical Engine**: [OpenAIRAC Core](https://github.com/bobberdolle1/open-airac)
