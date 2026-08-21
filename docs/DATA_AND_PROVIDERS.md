# 🗄️ Data & Providers Guide

This guide explains how navigation data works in OpenAIRAC, how AIRAC cycles are managed, and how you can manage data sources using the **Data & Providers Manager** and the **Local AIP Vault**.

---

## 📅 What is an AIRAC Cycle?

Aeronautical information changes globally on a strict **28-day cycle** established by ICAO (International Civil Aviation Organization). Each cycle is identified by a 4-digit number where the first two digits represent the year and the last two represent the cycle number (e.g. `2608` = August 2026).

OpenAIRAC is a **temporal database engine**:
* It tracks effective dates for every waypoint, airway, and procedure.
* It can preload the upcoming AIRAC cycle before it becomes active.
* It never invents or guesses missing data: missing procedures are safely flagged as `SOURCE_REQUIRED` rather than fabricating fictitious routes.

---

## 🌍 Data Federation Model

OpenAIRAC categorizes data into clear legal and provenance tiers:

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        OpenAIRAC Data Architecture                     │
├───────────────────────────────────┬────────────────────────────────────┤
│     Public Baseline (Bundled)     │    Local AIP Vault (User-Supplied) │
│                                   │                                    │
│  - US FAA CIFP (Procedures & ATS) │  - Russian CAICA AIP Packages      │
│  - OurAirports (Worldwide basic)  │  - German DFS AIP Datasets         │
│  - OpenFlightmaps (European VFR)  │  - Eurocontrol EAD Files           │
│  - France SIA eAIP (Terminal AD)  │  - Personal AIXM 5.x / CIFP Files  │
│                                   │                                    │
│  ✓ Active out-of-the-box          │  ✓ Stored privately on user machine│
│  ✓ 100% Free & Redistributable    │  ✓ Never published by OpenAIRAC    │
└───────────────────────────────────┴────────────────────────────────────┘
```

---

## 🛡️ Distribution & Copyright Policy

OpenAIRAC strictly enforces licensing rules:
* **Bundled Datasets**: Only datasets with confirmed open redistribution permissions (public domain, CC0, Open Data) are included in official releases.
* **Local-Only (BYOD)**: Datasets from national authorities that permit personal simulation use but forbid public redistribution (e.g. Russian CAICA) are supported **exclusively** through the user's local machine via the **Local AIP Vault**.
* **Strictly Forbidden**: Commercial proprietary payloads (**Navigraph**, **Jeppesen**, **NavDataPro**) are **NEVER** ingested, bundled, or redistributed by OpenAIRAC.

---

## 🖥️ Using the Data & Providers Manager

To inspect active providers or manage datasets:
1. Open **OpenAIRAC Map**.
2. Go to **OpenAIRAC → Data & Providers Manager...** in the top menu.
3. You will see the active provider table with status indicators:

| Status Indicator | Meaning |
|---|---|
| 🟢 **READY** | Navigation data is active, verified, and ready for flight planning. |
| 🟡 **SOURCE REQUIRED** | Airport is known, but official terminal procedures are not available in the public baseline (import required via Local AIP Vault). |
| 🟠 **STALE** | Active dataset has expired past its 28-day AIRAC validity window. |
| 🔵 **FUTURE** | Dataset is scheduled for an upcoming effective date and will activate automatically. |
| ⚪ **NO DATA** | No navigation records exist for this region. |
| 🔴 **FAILED** | Database corruption or schema mismatch (re-download or reset required). |

---

## 🔐 The Local AIP Vault

The **Local AIP Vault** is a private, encrypted folder on your computer where you can import lawful, officially acquired national AIP packages without violating redistribution terms.

### How to Import Data into the Vault:
1. Obtain an official AIP file (e.g. AIXM 5.x XML, ARINC 424 CIFP).
2. Open **OpenAIRAC → Data & Providers Manager**.
3. Switch to the **Local AIP Vault** tab and click **Import Dataset...**.
4. Select your file. OpenAIRAC will validate semantic continuity, sequence fixes, and runway bindings.
5. Click **Activate**. Your personal data is now immediately active across all flight planning and EFB views.
