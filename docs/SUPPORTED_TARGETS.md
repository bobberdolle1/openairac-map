# Supported Targets

Machine-enforced support matrix for every installable target product.
Support states are declared in `crates/openairac-export/src/lib.rs`
(`TargetDescriptor::support_state`) and are honest: **SUPPORTED**
requires the full path (export + format validation + transactional
install + post-install validation + rollback) to have passed against a
real data cycle.

Run `openairac target list` and `openairac target detect` for the
current registry and local detection results.

## Matrix (develop/1.1)

| Target | Family | State | Install | Detection | Verified evidence |
|---|---|---|---|---|---|
| `xplane12` | `xplane-dat` | **SUPPORTED** | Transactional layer install into `Custom Data` (backup, journal, semantic resolve, rollback) | `%XPLANE%` env, Steam (Windows/Linux), `C:\X-Plane 12`, `/Applications/X-Plane 12` | Golden vs convert424toxplane v12.4 (cycles 2608/2609); **live verified** on the local X-Plane 12 install (install + resolve consistent + rollback round-trip, cycle 2609) |
| `xplane11` | `xplane-dat` | **SUPPORTED** | Same layer mechanics | Same roots for XP11 | Same format family; verified compatibility |
| `little-navmap` | `little-navmap-sqlite` | **SUPPORTED** | Single-file DB copy into LNM database dir (transactional) | `%LNM_DATABASES%` env, `%APPDATA%\ABarthel\little_navmap_db` (Win), `~/.config/ABarthel/little_navmap_db` (Linux), `~/Library/Application Support/ABarthel/little_navmap_db` (macOS) | Schema + referential integrity vs atools schema v14.29; real-cycle export loads in sqlite tooling |
| `xplane-gns430` | `gns430-text` | **EXPERIMENTAL** | Transactional subdirectory install into `%XPLANE%/Custom Data/GNS430/navdata` | `%XPLANE%` env, Steam (Windows/Linux), `C:\X-Plane 12` | Full legacy GNS430 schema implemented & tested (Airports, Navaids, Waypoints, ATS, Proc/ per-airport files). Live in-cockpit GNS430 load test required before declaring SUPPORTED. |
| `kln90b` | `kln90b-dat` | **EXPERIMENTAL** | Transactional subdirectory install into KLN90B navdata root | `%KLN90B_NAVDATA%` env, `C:\KLN90B\navdata` | Clean-room MIT `.DAT` tables implemented (APT, NAV, WPT, AWY, FAS). PSS/PTT binary rebuild validation against unmodified Project Tupolev plugin required before declaring SUPPORTED. |
| `msfs2024` / `msfs2020` | `msfs-bgl` | **EXPERIMENTAL** | Whole-package transactional install into the Community folder | `%MSFS2024_COMMUNITY%` / `%MSFS_COMMUNITY%` env, MS Store `%LOCALAPPDATA%\Packages\...`, Steam `%APPDATA%\Microsoft Flight Simulator...` | Full ARINC 424 → BGLComp leg mapping (all published path terminators); real cycle 2609 export (199,966 legs); `fspackagetool.exe` compile requires a real SDK (fail-closed otherwise) |
| `pmdg-legacy` | `pmdg-text` | **EXPERIMENTAL** | File set copy (transactional) | `%PMDG_NAVDATA%` env, `C:\PMDG\NAVDATA`, MSFS Community `Config\NavData` | AIRNAV Navdata Data File Definition (public doc) + PMDG Navdata Technical Glossary; real cycle 2609 export of wpNavAPT/AID/FIX/RTE |
| `aerosoft-crj` | `navdatapro-text` | **RESEARCH** | none | none | No public vendor specification or open reference exists; blocked by the implementation-authority rule (see FORMATS.md) |
## Rollback semantics

- `openairac target rollback <id> [--path]` undoes the **last
  successful install**: the previous layer is retained (journal + backup
  kept at commit) and restored byte-identically. A second rollback is an
  idempotent no-op.
- Interrupted installs self-heal: any journal left behind is recovered
  (previous state restored) before the next install proceeds.
- Installers are cross-volume safe (Windows): staged files are copied
  into the target directory first, then swapped by same-volume rename.

## Multi-target updates

`openairac target update-all --db <world.sqlite> [--date ...]
[--min-state ...]` exports the world once per format family and updates
every detected target transactionally. Per-target failure rolls back
only that target and is reported; worlds are never mixed.

## Verification notes (2026-08-19, cycle 2609)

- Live X-Plane 12 (`F:\SteamLibrary\steamapps\common\X-Plane 12`):
  detection, cross-volume install, semantic resolve (consistent), and
  rollback round-trip verified. The live Custom Data currently carries
  the OpenAIRAC 2609 layer.
- Real-cycle release-build export timings: xplane 2.4s, little-navmap
  0.8s, msfs 17.0s, pmdg 0.2s (N+1 runway query and per-row scans
  eliminated).
