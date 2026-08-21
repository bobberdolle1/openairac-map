# 🔧 Troubleshooting Guide

This guide provides actionable solutions for common issues encountered when installing, configuring, and using **OpenAIRAC Map**.

---

## 🪟 1. Windows SmartScreen or Antivirus Warning

### Symptom:
When running `OpenAIRAC-Map-2.3.0-Setup.exe` on Windows, SmartScreen displays: *"Windows protected your PC — Microsoft Defender SmartScreen prevented an unrecognized app from starting."*

### Cause:
OpenAIRAC Map is an open-source project and is not signed with an expensive commercial EV Code-Signing certificate. New releases temporarily show this standard Windows warning until enough reputation is established.

### Solution:
1. Click **More info**.
2. Click **Run anyway**.
3. You can verify the integrity of the downloaded file against the official [SHA256SUMS.txt](https://github.com/bobberdolle1/openairac-map/releases/latest/download/SHA256SUMS.txt) in PowerShell:
   ```powershell
   Get-FileHash .\OpenAIRAC-Map-2.3.0-Setup.exe -Algorithm SHA256
   ```

---

## 🧩 2. Missing DLL Error on Startup

### Symptom:
`littlenavmap.exe` fails to start with: *"The code execution cannot proceed because VCRUNTIME140.dll / MSVCP140.dll was not found."*

### Solution:
Install the official Microsoft Visual C++ 2015–2022 Redistributable (x64):
* [Microsoft Visual C++ Redistributable (Official Link)](https://aka.ms/vs/17/release/vc_redist.x64.exe)

---

## 🎮 3. Simulator Connection Problems

### Symptom A: X-Plane 12/11 Not Detected
1. Verify that the `Little Xpconnect` plugin folder is copied into `<X-Plane 12>/Resources/plugins/`.
2. Check X-Plane's `Log.txt` to confirm `Little Xpconnect` loaded successfully without error.
3. In OpenAIRAC Map, go to **Tools → Connect to Flight Simulator**, select **X-Plane**, and click **Connect**.

### Symptom B: MSFS Connection Failed
1. Ensure MSFS is completely loaded into the 3D cockpit before clicking **Connect**.
2. Make sure no other program is exclusively blocking SimConnect.

---

## 🌐 4. Weather or VATSIM / IVAO Traffic Offline

### Symptom:
Airport weather shows *"NOAA Weather Unavailable"* or no VATSIM aircraft appear on the map.

### Solution:
1. Check your internet connection.
2. In OpenAIRAC Map, go to **Tools → Options → Weather** and ensure NOAA AviationWeather.gov is selected.
3. In **Tools → Options → Online Flying**, ensure **VATSIM** or **IVAO** is enabled.
4. If your network uses a proxy or custom firewall, ensure `aviationweather.gov` and `data.vatsim.net` are accessible over HTTPS.

---

## 🚪 5. AI Crew Gateway Port 8989 Conflict

### Symptom:
Logs indicate: *"Failed to bind AI Crew Gateway: Address already in use (os error 10048)"*.

### Solution:
Another process is using TCP port 8989.
1. In Windows PowerShell, check what process is holding port 8989:
   ```powershell
   Get-NetTCPConnection -LocalPort 8989
   ```
2. Terminate the conflicting background process or close any orphaned instances of OpenAIRAC.

---

## 📂 6. Where are Log Files Located?

OpenAIRAC Map writes detailed diagnostic logs to help troubleshoot crashes or errors:

* **In-App Quick Access**: Go to **OpenAIRAC → Open Log Folder** in the top menu.
* **Windows Path**:
  ```text
  %LOCALAPPDATA%\Temp\abarthel-little_navmap.log
  %APPDATA%\ABarthel\
  ```
* **Attaching Logs to Bug Reports**: When reporting an issue on GitHub, please attach `abarthel-little_navmap.log` (sensitive information and private keys are never written to logs).

---

## 🔄 7. Full Application Reset (Clean Slate)

If settings or caches become corrupted:
1. Close OpenAIRAC Map.
2. Open Windows File Explorer and navigate to:
   ```text
   %APPDATA%\ABarthel
   ```
3. Rename or delete `little_navmap.ini` and `little_navmap_db`.
4. Relaunch OpenAIRAC Map. The First-Run Wizard will restart with default settings.
