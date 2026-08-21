# 🎮 Simulator Setup & Connection Guide

OpenAIRAC Map connects natively to your flight simulator to provide real-time moving map tracking, aircraft position, airspeed, altitude profiles, and wind telemetry.

---

## 📊 Simulator Support Matrix

| Simulator | Connection Method | Support Status | Capabilities |
|---|---|---|---|
| **X-Plane 12** | Little Xpconnect plugin (local) / UDP | **SUPPORTED + LIVE TESTED** | Position, heading, altitude, GS/IAS, flight plan sync, weather |
| **X-Plane 11** | Little Xpconnect plugin (local) | **SUPPORTED + LIVE TESTED** | Position, heading, altitude, GS/IAS, flight plan sync |
| **MSFS 2024** | SimConnect (Direct) | **SUPPORTED + PROTOCOL TESTED** | Position, altitude, groundspeed, AI traffic |
| **MSFS 2020** | SimConnect (Direct) | **SUPPORTED + LIVE TESTED** | Position, altitude, groundspeed, AI traffic |
| **Prepar3D v4/v5** | SimConnect (Direct) | **SUPPORTED + PROTOCOL TESTED** | Position, altitude, groundspeed, AI traffic |
| **Standalone Mode** | None (No sim connection) | **SUPPORTED + LIVE TESTED** | Standalone flight planning & offline map review |

---

## 🛩️ Setting Up X-Plane 12 / 11

### 1. Install the Plugin (One-Time Setup)
OpenAIRAC Map includes the **Little Xpconnect** plugin inside the distribution folder:
1. Locate the `Little Xpconnect` directory inside your OpenAIRAC Map installation folder.
2. Copy the entire `Little Xpconnect` folder into:
   ```text
   <X-Plane 12 Path>/Resources/plugins/
   ```
3. Your folder structure should look like:
   ```text
   X-Plane 12/
   └── Resources/
       └── plugins/
           └── Little Xpconnect/
               ├── 64/
               │   └── win.xpl (mac.xpl / lin.xpl)
               └── ...
   ```

### 2. Connect
1. Start **X-Plane 12** and load any flight.
2. In OpenAIRAC Map, select **Tools → Connect to Flight Simulator** (or press `Ctrl+Shift+C`).
3. Select the **X-Plane** tab, ensure **Connect automatically** is checked, and click **Connect**.
4. The status bar at the bottom will turn green with **Connected: X-Plane**.

---

## ✈️ Setting Up Microsoft Flight Simulator (MSFS 2024 / 2020)

### 1. Requirements
* MSFS uses native **SimConnect** — no external plugin installation is required.
* OpenAIRAC Map connects automatically when MSFS is running on the same computer.

### 2. Connect
1. Start **MSFS** and enter the cockpit.
2. In OpenAIRAC Map, select **Tools → Connect to Flight Simulator**.
3. Select the **Microsoft Flight Simulator / FSX / P3D** tab and click **Connect**.
4. The status bar will show **Connected: SimConnect**.

---

## 🌐 Remote / Networked Laptop Setup (Little Navconnect)

If you run OpenAIRAC Map on a second computer (such as a laptop or tablet):
1. On the **simulator PC**, run `Little Navconnect.exe` (included in the release package).
2. Note the IP address displayed in Little Navconnect (e.g. `192.168.1.50`).
3. On your **laptop**, open OpenAIRAC Map, go to **Tools → Connect to Flight Simulator**, choose **Connect to a remote flight simulator**, enter the simulator PC's IP address, and click **Connect**.

---

## 🔧 Troubleshooting Connection Issues

* **"Plugin not found in X-Plane"**: Verify that `Little Xpconnect` is directly in `X-Plane 12/Resources/plugins/` (not nested inside duplicate subfolders).
* **"SimConnect connection failed"**: Ensure MSFS is fully loaded into the 3D cockpit before connecting.
* **Windows Firewall Prompt**: Allow `littlenavmap.exe` and `littlenavconnect.exe` through the Windows Defender Firewall on private networks.
* **Mac Quarantine Flag**: If running on macOS, clear the quarantine attribute for `Little Xpconnect` in Terminal:
  ```bash
  xattr -r -d com.apple.quarantine "/path/to/X-Plane 12/Resources/plugins/Little Xpconnect"
  ```
