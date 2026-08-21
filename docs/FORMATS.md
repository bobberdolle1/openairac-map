# Format & Target Provenance Matrix

Record of format families OpenAIRAC can produce, the implementation
authority for each, and the honest support state per target. Nothing
here is claimed without an executed export+install+validation path.

## Shipped exporters

| Family | Format | Authority | State |
|---|---|---|---|
| xplane-dat | XPNAV1200 / XPFIX1200 / XPAWY1101 | Official Laminar specs (v1200/1101), golden-verified vs convert424toxplane v12.4 on cycles 2608+2609 | **Supported** (X-Plane 12), Experimental (X-Plane 11) |
| xplane-cifp1250 | CIFP/$ICAO.dat terminal procedures | Official Laminar XP-CIFP1250 spec; production encoder = Laminar's convert424toxplane (we feed it the FAA CIFP) | Production path documented (X_PLANE_STRATEGY.md); converter is the sanctioned encoder |
| msfs-bgl | SimpleNavData-style BGL package sources | Official MSFS SDK path (bglcomp schema, SimpleNavData sample, fspackagetool.exe, PackageOrderHint CUSTOM_NAVDATA); full ARINC 424 -> BGLComp leg mapping (all published path terminators, 199,966 legs on cycle 2609) | **Experimental** (source generation + transactional Community install verified; SDK compile + BglExplorer verification pending a real SDK) |
| little-navmap-sqlite | Little Navmap nav database | Open-source schema (albar965/atools, GPL-3.0; interface reference only) | **Supported** for install path; in-app load not executed on the verification machine (Little Navmap absent) |
| pmdg-text | wpNavAPT / wpNavAID / wpNavFIX / wpNavRTE | AIRNAV Navdata Data File Definition (public document) + PMDG Navdata Technical Glossary | **Experimental** (real cycle 2609 export verified; not loaded in a PMDG aircraft) |
| gns430-text | Airports.txt / Navaids.txt / Waypoints.txt / ATS.txt / Proc/<ICAO>.txt | Public Garmin GNS430 & X-Plane Custom Data layout | **Experimental** (complete schema implemented and verified; live in-cockpit simulated GPS execution pending) |
| kln90b-dat | APT.DAT / NAV.DAT / WPT.DAT / AWY.DAT / FAS.DAT / cycle.dat | Clean-room MIT implementation based on open KLN90B GPS loaders (Tu-154, vasFMC) | **Experimental** (clean-room generator implemented; binary PTT cache rebuild validation pending) |
## Research-only families (no implementation yet)

| Family | Target examples | Blocking reason |
|---|---|---|
| navdatapro-text | Aerosoft CRJ / NavDataPro | No official vendor specification or open-source reference; community docs are observational only. Implementing from the local commercial package alone would violate the implementation-authority rule. |
| lnm-flightplans, leveld-xml, feelthere-text, flight1-text | Level-D, FeelThere/Wilco/KLN90B, Flight1/FSBuild | Authority not yet established; per-family clean-room or open reference required. |
| dfd-sqlite, fwd-sqlite, fenix/tfdi | DFD / FWD-FD / Fenix / TFDi | Container compatibility is observed-only (proprietary); aviation semantics would be independently grounded, but serializer implementation requires clean-room proof per the observed-container policy. |

## Policy

1. **Exporter != installer.** A format family is produced once; targets
   differ only in install roots/validation, expressed declaratively in
   `TargetDescriptor`s (see `crates/openairac-export`).
2. **No proprietary navdata is used, copied, or reverse-engineered.**
   The local commercial distribution may serve only as an
   observational compatibility oracle, never as an implementation
   source or test fixture.
3. **Fail closed.** Any field a target format cannot represent is
   skipped with a diagnostic — never fabricated, never re-shaped.
4. **Support claims require the full path:** export + format
   validation + transactional install + post-install validation +
   rollback, machine-enforced by the release gate. The registry in
   `crates/openairac-export` reflects exactly this.
