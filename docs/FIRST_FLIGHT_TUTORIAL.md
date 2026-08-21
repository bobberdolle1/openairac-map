# 🚀 First Flight Tutorial

This tutorial walks you through planning, verifying, and flying your first complete IFR flight in **OpenAIRAC Map** using **100% free, bundled public navigation data**.

No external accounts, subscriptions, or imports are required!

---

> **Scenario**: Paris Charles de Gaulle (`LFPG`) to Nice Côte d'Azur (`LFMN`)  
> **Aircraft**: Airbus A320 / Boeing 737 / Generic Jet  
> **Cruise Altitude**: FL330 (33,000 ft)  
> **Estimated Time Enroute**: ~1h 15m

---

## Step 1: Launch OpenAIRAC Map

1. Start **OpenAIRAC Map** from your Start Menu or desktop shortcut (or run `littlenavmap.exe`).
2. On first launch, the **First-Run Setup Wizard** will verify that the public navigation database is active. Click **Finish**.

---

## Step 2: Set Origin and Destination

1. Open the **Flight Planning** dock on the left side of the window.
2. In the **Departure** field, type: `LFPG` (Paris Charles de Gaulle).
3. In the **Destination** field, type: `LFMN` (Nice Côte d'Azur).
4. Set the **Cruise Altitude** to `33000` ft (or `FL330`).

---

## Step 3: Calculate the Route

1. In the Flight Planning dock toolbar, click the **Calculate Flight Plan** button (or press `Ctrl+Alt+C`).
2. Select **Calculate along Airways (High Altitude)** and click **Calculate**.
3. OpenAIRAC Map automatically computes a fuel-optimal route through official French and European airway corridors connecting departure and arrival fixes.

---

## Step 4: Assign Runway & Procedures

1. **Departure SID**:
   - Right-click `LFPG` on the map or in the flight plan table, and select **Show Departure Procedures**.
   - Select departure runway (e.g. `Runway 26L`) and the official SID procedure (e.g. `OPALE 5A`).
   - Click **Insert SID into Flight Plan**.
2. **Arrival Approach & STAR**:
   - Right-click `LFMN` and select **Show Arrival Procedures**.
   - Select arrival runway (e.g. `Runway 04L`) and the official approach (e.g. `RNP 04L` or `ILS 04L`).
   - Click **Insert Approach into Flight Plan**.

Your full flight plan is now continuously connected from takeoff to landing with 0 discontinuities!

---

## Step 5: Check Weather & Official Charts

1. **Weather Briefing**:
   - Go to **OpenAIRAC → Flight Briefing [Weather & Traffic]**.
   - Inspect live METAR observations and wind components for `LFPG` and `LFMN`.
2. **View Official Approach Plates**:
   - Open the **Charts** dock (`OpenAIRAC → Charts Viewer`).
   - Search for `LFMN` to display the official France SIA instrument approach plate directly on your screen.

---

## Step 6: Connect to Your Simulator

1. Launch your simulator (e.g., **X-Plane 12** or **MSFS 2024 / 2020**).
2. Position your aircraft at a gate in `LFPG`.
3. In OpenAIRAC Map, go to **Tools → Connect to Flight Simulator**.
4. The status bar will show **Connected: X-Plane** or **Connected: SimConnect**.
5. Your aircraft symbol will appear on the map at Paris Charles de Gaulle with real-time heading and coordinates!

---

## Step 7: Export the Flight Plan

1. In the menu, go to **File → Export Flight Plan As**:
   - For **X-Plane 12/11**: Select **Export Flight Plan as X-Plane 11/12 FMS (.fms)** and save to `Output/FMS plans/`.
   - For **MSFS**: Select **Export Flight Plan as MSFS 2020/2024 PLN (.pln)**.
   - For **PMDG**: Select **Export Flight Plan as PMDG RTE (.rte)**.
2. Load the flight plan directly in your aircraft's FMC or GPS and take off!

---

## 🎯 What's Next?

* Learn how to connect online networks: [Simulator Setup Guide](SIMULATOR_SETUP.md)
* Understand provider data policies: [Data & Providers Guide](DATA_AND_PROVIDERS.md)
* Learn how to import optional Russian AIP data: [Russia / CAICA Guide](RUSSIA_CAICA_GUIDE.md)
