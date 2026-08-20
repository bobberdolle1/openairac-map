# OpenAIRAC Map Troubleshooting & FAQ

## 1. Frequently Asked Questions

### Q: Why do I see 0 terminal procedures for some European airports (e.g. LFPG)?
**A:** OpenAIRAC strictly complies with official open data licences. The public French SIA export provides all aerodromes, runways, navaids, and enroute airways, but does not provide machine-readable SIDs/STARs in public XML. However, official Section AD 2.24 eAIP chart plates are fully available in the **Charts** tab!

### Q: My simulator connection is not establishing.
**A:**
* For **X-Plane**: verify that `XpConnect` plugin is enabled in `Resources/plugins`.
* For **MSFS**: verify that SimConnect runtime is installed.
* Run **Tools > OpenAIRAC Diagnostics...** to verify simulator paths.

### Q: How do I report an issue or bug?
**A:**
1. Open **Tools > OpenAIRAC Diagnostics...**
2. Click **Copy Diagnostic Report**.
3. Create an issue on GitHub (`https://github.com/bobberdolle1/openairac-map/issues`) and paste the sanitized report.
