# OpenAIRAC Installation & User Guide

This guide explains how to install, configure, and use **OpenAIRAC 1.0.0**.

> **FLIGHT SIMULATION ONLY.** OpenAIRAC is not certified and must never be used
> for real-world aviation.

---

## FAQ & User Quick Reference

### 1. What is OpenAIRAC?
OpenAIRAC is an open navigation-data engine for flight simulators. It ingests
public navigation data (FAA CIFP, OurAirports), maintains a canonical
temporal store, and exports validated simulator datasets.

### 2. Which simulators and formats are supported?
- **SUPPORTED:** X-Plane 12 (navdata layer: fixes, navaids, airways, transactional install, post-validation, rollback). Terminal procedures use the official `convert424toxplane` tool (see `docs/X_PLANE_STRATEGY.md`).
- **EXPERIMENTAL:** X-Plane 11, MSFS 2020 / 2024 (official SDK `SimpleNavData` package source generation + Community folder transactional install), Little Navmap (SQLite nav database).
- **RESEARCH ONLY:** Aerosoft CRJ, PMDG, Level-D, FeelThere, Flight1, DFD, Fenix (see `docs/FORMATS.md`).

### 3. How do I check which cycle I am on?
- Check your local database:
  ```powershell
  openairac-cli.exe status --db .\data\world.openairac.sqlite
  ```
- Detect simulator Custom Data layer status:
  ```powershell
  openairac-cli.exe export detect
  ```

### 4. What will installation change?
Installing into X-Plane writes four files into `<X-Plane 12>/Custom Data`:
- `earth_fix.dat` (waypoints/fixes)
- `earth_nav.dat` (navaids, localizers, glideslopes, DMEs)
- `earth_awy.dat` (airways)
- `openairac_layer.json` (layer identity and verification fingerprint)

Any existing files in `Custom Data` are automatically backed up before swapping.

### 5. How do I install into X-Plane 12?
```powershell
# Transactionally install the current active cycle into X-Plane 12 Custom Data:
openairac-cli.exe export xplane --db .\data\world.openairac.sqlite --out .\dist\xplane --install-to "F:\SteamLibrary\steamapps\common\X-Plane 12\Custom Data"
```

### 6. How do I update to a new cycle?
```powershell
# 1. Discover and fetch the new cycle:
openairac-cli.exe cycle discover --db .\data\world.openairac.sqlite
openairac-cli.exe sync --provider faa_cifp --cycle 2609 --db .\data\world.openairac.sqlite

# 2. Export and transactionally install when effective:
openairac-cli.exe export xplane --db .\data\world.openairac.sqlite --out .\dist\xplane --install-to "<Custom Data>"
```

### 7. How do I roll back?
If a failure occurs during install, rollback is automatic. To roll back an installed cycle manually:
```powershell
# Roll back an active cycle in the database:
openairac-cli.exe cycle rollback --cycle 2609 --db .\data\world.openairac.sqlite
```
For bundle-level installs:
```powershell
openairac-cli.exe bundle rollback --root .\install_root
```

### 8. Where are backups stored?
During transactional installation, pre-existing files are placed in:
`<Target Directory>/.openairac_backup_<operation_id>/`
On successful commit, the temporary backup directory is cleaned up.

### 9. How do I uninstall / remove OpenAIRAC?
Simply delete the four files (`earth_fix.dat`, `earth_nav.dat`, `earth_awy.dat`, `openairac_layer.json`) from `<X-Plane 12>/Custom Data`. X-Plane will seamlessly revert to its default factory navdata in `<X-Plane 12>/Resources/default data`.

### 10. How do I verify a bundle?
```powershell
# Verify bundle integrity against the embedded production trust root:
openairac-cli.exe bundle verify --bundle .\bundles\077f88178ff700d6

# Verify a signed release checksum file:
openairac-cli.exe keygen verify-file --public-key openairac-prod.pub --file SHA256SUMS.txt --signature SHA256SUMS.txt.sig
```

### 11. What happens when the next AIRAC cycle becomes effective?
OpenAIRAC uses strict temporal validity (`valid_from` <= query instant in UTC). Preloaded future data remains inactive until the exact second of its confirmed effective instant (`09:01:00Z` on the cycle effective date). Queries and exports automatically switch to the newly effective cycle without timezone ambiguity or manual intervention.

---

## Building from Source

```bash
git clone https://github.com/bobberdolle1/open-airac.git
cd open-airac

# Run full test suite
cargo test --workspace --all-features

# Run release gate
scripts/release-gate.sh
```
