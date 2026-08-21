# 💾 Saving & Exporting Flight Plans

OpenAIRAC Map allows you to save and export your calculated routes and terminal procedures to all major flight simulators, add-on aircraft avionics, and GPS units.

---

## 📁 Native Flight Plan Format (`.lnmpln`)

* **Menu**: `File → Save Flight Plan` (or `Ctrl+S`)
* **Format**: XML-based `.lnmpln`
* **Features**: Preserves all waypoint coordinates, assigned SIDs/STARs/IAPs, runway selections, cruise altitudes, aircraft performance profiles, and user remarks.
* **Default Directory**: `Documents/Little Navmap Files/Flight Plans/` (or user-configured flight library).

---

## 🛫 Supported Export Formats & Simulator Paths

To export to your simulator or aircraft avionics, go to **File → Export Flight Plan As**:

| Target Format | File Extension | Default Destination Path | Compatible Aircraft & Avionics |
|---|---|---|---|
| **X-Plane 12 / 11 FMS** | `.fms` | `<X-Plane 12>/Output/FMS plans/` | Default X-Plane FMS, Zibo 737, Toliss Airbus, Flight Factor |
| **MSFS 2024 / 2020 PLN** | `.pln` | `<MSFS LocalState>/` or `Documents/` | MSFS World Map, FlyByWire A32NX, Fenix A320, Inibuilds |
| **PMDG Classic FMC** | `.rte` | `<Sim Path>/PMDG/FLIGHTPLANS/` | PMDG 737, 777, 747 |
| **Garmin GNS 430/530** | `.fpl` | `<Sim Path>/Garmin/FPL/` | Reality XP GNS, standard Garmin GPS units |
| **Reality XP GTN** | `.gfp` | `Documents/Reality XP/` | Reality XP GTN 750 / 650 |
| **Flight1 GTN** | `.gfp` | `Documents/Flight1/` | Flight1 GTN 750 / 650 |
| **Aerosoft Airbus/CRJ** | `.flp` | `<Sim Path>/Aerosoft/FlightPlans/` | Aerosoft Airbus & CRJ series |
| **iFly 737** | `.fltplan`| `<Sim Path>/iFly/737NG/navdata/FLTPLAN/` | iFly Boeing 737 |
| **GPX Exchange** | `.gpx` | Any directory | External GPS tools, Google Earth, tablets |

---

## ⚡ Multiexport (One-Click Export)

If you fly multiple simulators or add-on aircraft:
1. Go to **File → Multiexport Flight Plan Options...**.
2. Select all formats and destination folders you wish to export simultaneously (e.g. X-Plane `.fms` + PMDG `.rte` + MSFS `.pln`).
3. Whenever you plan a flight, simply press **Shift+Ctrl+M** (or select **File → Multiexport Flight Plan**). All files are generated and placed in their respective simulator directories in one click!

---

## 🔄 Loading Exported Plans in Your Cockpit

* **X-Plane FMS**: In the aircraft FMC/MCDU, go to `CO RTE` or `ROUTE → LOAD` and type your flight plan name (e.g. `LFPGLFBO`).
* **MSFS World Map**: On the World Map screen, press the **Load / Save** button (or `Space`), choose **Load**, and select your exported `.pln` file.
* **Garmin GPS**: In the GPS unit, press the `FPL` page button, scroll to the Flight Plan Catalog, and select your flight plan.
