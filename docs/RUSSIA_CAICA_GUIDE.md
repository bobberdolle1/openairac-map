# 🇷🇺 Russia / CAICA Local Import Guide

This guide explains how users can import official Russian civil aviation navigation data (**CAICA** / ЦАИ ГА) into OpenAIRAC Map using the **Local AIP Vault**.

---

## ⚖️ Why CAICA Data is Not Bundled

The State Air Traffic Management Corporation / CAICA publishes official aeronautical navigation data for the Russian Federation. While individual flight simulation enthusiasts can access these files for personal use, **CAICA does not grant third-party public redistribution rights**.

To strictly respect copyright and state aeronautical regulations:
* OpenAIRAC **never** bundles raw CAICA packages in public downloads.
* OpenAIRAC **never** bypasses login or access controls.
* Users who lawfully possess official CAICA datasets can import them **locally** on their own PC for personal flight simulation.

---

## 📥 Prerequisites

You will need official CAICA aeronautical source files:
* CAICA ATS Route Manual (Маршруты ОВД) / Enroute tables
* CAICA Aerodrome & Terminal Procedure Packages (Аэродромы и процедуры маневрирования)
* Format: AIXM 5.x, ARINC 424, or CAICA XML/PDF structured tables

---

## 🛠️ Step-by-Step GUI Import Process

### 1. Open Data & Providers Manager
1. Launch **OpenAIRAC Map**.
2. Go to **OpenAIRAC → Data & Providers Manager...**.
3. Select the **Local AIP Vault** tab.

### 2. Import Your CAICA Package
1. Click the **Import Local Package...** button.
2. Select your CAICA dataset file or directory.
3. OpenAIRAC will parse the dataset and perform automated validation:
   - Verifies fix continuity and magnetic variation.
   - Checks runway threshold bindings.
   - Validates ARINC 424 leg transitions (e.g. `BURUD 2Y` STAR into `URFF` Runway 19R).

### 3. Activate the Russian Dataset
1. Click **Activate Dataset**.
2. The provider status for Russia will change from 🟡 `SOURCE_REQUIRED` to 🟢 `READY (CAICA Local)`.

---

## 🛫 Verifying the Import (Example: UUEE -> URFF)

Once activated, you can immediately plan routes across Russian airspace:
1. Open the **Flight Planning** dock.
2. Set Departure to `UUEE` (Moscow Sheremetyevo) and Destination to `URFF` (Simferopol).
3. Click **Calculate Flight Plan**.
4. OpenAIRAC will construct a continuous route along official ATS corridors (e.g. `EMGAS -> W109 -> BURUD`).
5. In the terminal procedure selector, assign STAR `BURUD 2Y` and Approach `ILS 19R`.

---

## 🔄 Rollback & Reset

If you ever wish to remove or update your local CAICA dataset:
1. Go to **OpenAIRAC → Data & Providers Manager → Local AIP Vault**.
2. Select the installed Russian dataset and click **Deactivate** or **Remove from Vault**.
3. OpenAIRAC immediately reverts to the clean public baseline.
