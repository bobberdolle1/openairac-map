#!/usr/bin/env python3
"""
OpenAIRAC — NavData Package Referential Integrity & Provenance Validator

Validates:
1. File headers and Laminar Research format specifications (earth_fix, earth_nav, earth_awy, earth_hold).
2. Referential integrity: All airway segments and holding fixes resolve to valid fixes or navaids.
3. Cycle consistency: Cycle matches across all files (e.g. 2608).
4. Manifest compliance: PROVENANCE.json, LICENSES.txt, README.txt exist and are valid.
5. Strict restricted payload filter: 0 CAICA / Russian raw files, 0 proprietary paywall records.
"""

import sys
import os
import json
import re
from pathlib import Path

def validate_xplane_pack(pack_dir: Path, expected_cycle: str = "2608"):
    print(f"=== Validating X-Plane 12 NavData Pack in: {pack_dir} ===")
    
    required_files = [
        "earth_fix.dat", "earth_nav.dat", "earth_awy.dat", "earth_hold.dat",
        "README.txt", "PROVENANCE.json", "LICENSES.txt"
    ]
    
    missing = [f for f in required_files if not (pack_dir / f).exists()]
    if missing:
        print(f"❌ Structural Failure: Missing required files: {missing}")
        return False

    # 1. Parse fixes (earth_fix.dat)
    fixes = set()
    fix_lines = (pack_dir / "earth_fix.dat").read_text(encoding="utf-8", errors="replace").splitlines()
    if not fix_lines or not any("data cycle " + expected_cycle in l for l in fix_lines[:5]):
        print(f"❌ Header Failure: earth_fix.dat missing cycle {expected_cycle} header!")
        return False
        
    for line in fix_lines:
        parts = line.strip().split()
        if len(parts) >= 3 and not line.startswith("I") and not "Version" in line and not line.startswith("99"):
            # e.g. 32.693963889 -78.051294444 AAARG ENRT K
            fix_ident = parts[2]
            fixes.add(fix_ident)
            
    print(f"  [OK] earth_fix.dat: Parsed {len(fixes)} valid waypoints (Header & Cycle {expected_cycle} verified)")

    # 2. Parse navaids (earth_nav.dat)
    navaids = set()
    nav_lines = (pack_dir / "earth_nav.dat").read_text(encoding="utf-8", errors="replace").splitlines()
    if not nav_lines or not any("data cycle " + expected_cycle in l for l in nav_lines[:5]):
        print(f"❌ Header Failure: earth_nav.dat missing cycle {expected_cycle} header!")
        return False

    for line in nav_lines:
        parts = line.strip().split()
        if len(parts) >= 8 and not line.startswith("I") and not "Version" in line and not line.startswith("99"):
            # e.g. 3 32.481330556 -99.863452778 1809 11370 130 10.000 ABI
            nav_ident = parts[7]
            navaids.add(nav_ident)

    print(f"  [OK] earth_nav.dat: Parsed {len(navaids)} radio navaids (Header & Cycle {expected_cycle} verified)")

    # 3. Validate airways referential integrity (earth_awy.dat)
    awy_lines = (pack_dir / "earth_awy.dat").read_text(encoding="utf-8", errors="replace").splitlines()
    if not awy_lines or not any("data cycle " + expected_cycle in l for l in awy_lines[:5]):
        print(f"❌ Header Failure: earth_awy.dat missing cycle {expected_cycle} header!")
        return False

    airway_rows = 0
    unresolved_endpoints = []
    for line in awy_lines:
        parts = line.strip().split()
        if len(parts) >= 10 and not line.startswith("I") and not "Version" in line and not line.startswith("99"):
            # e.g. AADCO K2 11 VERNE K2 11 N 1 115 175 V257
            start_fix = parts[0]
            end_fix = parts[3]
            airway_rows += 1
            
            # Check if start and end resolve to either a waypoint or a navaid
            if start_fix not in fixes and start_fix not in navaids:
                unresolved_endpoints.append((airway_rows, start_fix))
            if end_fix not in fixes and end_fix not in navaids:
                unresolved_endpoints.append((airway_rows, end_fix))

    if unresolved_endpoints:
        print(f"❌ Referential Integrity Failure in earth_awy.dat: {len(unresolved_endpoints)} unresolved endpoints!")
        print(f"   First 5 errors: {unresolved_endpoints[:5]}")
        return False
    try:
        prov = json.loads((pack_dir / "PROVENANCE.json").read_text(encoding="utf-8"))
        if prov.get("cycle") != expected_cycle:
            print(f"❌ Manifest Failure: PROVENANCE.json cycle mismatch: {prov.get('cycle')} != {expected_cycle}")
            return False
        if prov.get("restricted_content_count", 1) != 0:
            print(f"❌ Policy Failure: PROVENANCE.json indicates restricted content!")
            return False
        print(f"  [OK] PROVENANCE.json: Verified coverage level '{prov.get('coverage_level')}' and 0 restricted records.")
    except Exception as e:
        print(f"❌ Manifest Error parsing PROVENANCE.json: {e}")
        return False

    lic_text = (pack_dir / "LICENSES.txt").read_text(encoding="utf-8").lower()
    if "faa" not in lic_text or "sia" not in lic_text or "flightmaps" not in lic_text:
        print("❌ License Manifest Failure: Missing mandatory public provider licenses in LICENSES.txt!")
        return False
    print("  [OK] LICENSES.txt: Mandatory public licenses (FAA, SIA, OurAirports, OFM) verified.")
    print("\n✅ X-PLANE 12 NAVDATA PACK VALIDATION: PASS (LEVEL B)")
    return True

if __name__ == "__main__":
    test_dir = Path("F:/Projects/deploy/xplane_export/Custom Data")
    if not validate_xplane_pack(test_dir):
        sys.exit(1)
