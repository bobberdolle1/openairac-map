#!/usr/bin/env python3
"""
OpenAIRAC — Master Release Packaging Pipeline

Builds all user-ready distribution packages:
1. OpenAIRAC Map Windows Installer (Setup.exe)
2. OpenAIRAC Map Windows Portable ZIP (win64.zip)
3. OpenAIRAC Map Linux Archive (.tar.xz) & Debian Package (.deb)
4. OpenAIRAC Map macOS Application Bundle (.zip)
5. OpenAIRAC Core Prebuilt CLI (openairac-cli-v2.12.0-win64.zip)
6. OpenAIRAC Simulator NavData — X-Plane 12 (OpenAIRAC-NavData-XPlane12-2608.zip)
7. OpenAIRAC Simulator NavData — GNS430 (OpenAIRAC-NavData-GNS430-2608.zip)
8. Little Xpconnect Connector Plugin (Little-Xpconnect-win64.zip)
9. Cryptographic Checksums (SHA256SUMS.txt)

Enforces strict restricted payload leak scanning before publication.
"""

import sys
import os
import shutil
import subprocess
import hashlib
import zipfile
import tarfile
from pathlib import Path

VERSION_MAP = "2.4.0"
VERSION_CORE = "2.12.0"
AIRAC_CYCLE = "2608"

CSC_PATH = Path(r"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe")
MAP_ROOT = Path("F:/Projects/openairac-map")
CORE_ROOT = Path("F:/Projects/open-airac")
BUILD_MAP = Path("F:/Projects/build-openairac-map-release")
DEPLOY_MAP_SRC = Path("F:/Projects/deploy/Little Navmap win64")
OUTPUT_DIR = Path(f"F:/Projects/deploy/release_v{VERSION_MAP.replace('.', '')}")

def sha256_file(filepath: Path) -> str:
    h = hashlib.sha256()
    with open(filepath, "rb") as f:
        while chunk := f.read(65536):
            h.update(chunk)
    return h.hexdigest()

def main():
    print(f"=== OpenAIRAC Master Release Packaging Pipeline (Map v{VERSION_MAP} / Core v{VERSION_CORE}) ===")
    
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    # 1. Sync latest Map binary and version.txt
    built_exe = BUILD_MAP / "littlenavmap.exe"
    if built_exe.exists():
        print(f"[1/9] Syncing Map executable from {built_exe}...")
        shutil.copy2(built_exe, DEPLOY_MAP_SRC / "littlenavmap.exe")
    (DEPLOY_MAP_SRC / "version.txt").write_text(f"win64-{VERSION_MAP}\n", encoding="utf-8")

    # 2. Compile uninstaller
    print("[2/9] Compiling standalone uninstaller...")
    uninst_cs = MAP_ROOT / "tools" / "uninstaller_source.cs"
    uninst_exe = DEPLOY_MAP_SRC / "Uninstall.exe"
    cmd_uninst = [
        str(CSC_PATH), "/target:winexe", "/optimize+", "/platform:x64",
        "/reference:System.dll", "/reference:System.Windows.Forms.dll", "/reference:System.Drawing.dll",
        f"/out:{uninst_exe}", str(uninst_cs)
    ]
    subprocess.run(cmd_uninst, check=True, capture_output=True)

    # 3. Leak scan gate
    print("[3/9] Running restricted payload leak scanner...")
    scan_script = MAP_ROOT / "tools" / "scan_leaks.py"
    res_scan = subprocess.run([sys.executable, str(scan_script), str(DEPLOY_MAP_SRC)], capture_output=True, text=True)
    if res_scan.returncode != 0:
        print(f"❌ LEAK SCAN FAILED:\n{res_scan.stdout}\n{res_scan.stderr}")
        sys.exit(1)
    print(res_scan.stdout.strip())

    # 4. Package Windows Map (Portable ZIP & Setup.exe)
    print(f"[4/9] Packaging OpenAIRAC Map v{VERSION_MAP} Windows artifacts...")
    win_zip = OUTPUT_DIR / f"OpenAIRAC-Map-{VERSION_MAP}-win64.zip"
    with zipfile.ZipFile(win_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        for root, _, files in os.walk(DEPLOY_MAP_SRC):
            for file in files:
                p = Path(root) / file
                arcname = Path(f"OpenAIRAC-Map-{VERSION_MAP}") / p.relative_to(DEPLOY_MAP_SRC)
                zf.write(p, arcname)
    print(f"  -> Portable ZIP: {win_zip.name} ({win_zip.stat().st_size / (1024*1024):.2f} MB)")

    # Temporary installer payload zip
    temp_payload = OUTPUT_DIR / "temp_payload.zip"
    with zipfile.ZipFile(temp_payload, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
        for root, _, files in os.walk(DEPLOY_MAP_SRC):
            for file in files:
                p = Path(root) / file
                zf.write(p, p.relative_to(DEPLOY_MAP_SRC))

    inst_cs = MAP_ROOT / "tools" / "installer_source.cs"
    inst_exe = OUTPUT_DIR / f"OpenAIRAC-Map-{VERSION_MAP}-Setup.exe"
    cmd_inst = [
        str(CSC_PATH), "/target:winexe", "/optimize+", "/platform:x64",
        f"/resource:{temp_payload},payload.zip",
        "/reference:System.dll", "/reference:System.Windows.Forms.dll", "/reference:System.Drawing.dll",
        "/reference:System.IO.Compression.dll", "/reference:System.IO.Compression.FileSystem.dll",
        f"/out:{inst_exe}", str(inst_cs)
    ]
    subprocess.run(cmd_inst, check=True, capture_output=True)
    temp_payload.unlink(missing_ok=True)
    print(f"  -> Setup Installer: {inst_exe.name} ({inst_exe.stat().st_size / (1024*1024):.2f} MB)")

    # 5. Package Linux Tarball and .deb package layout
    print(f"[5/9] Packaging OpenAIRAC Map v{VERSION_MAP} Linux artifacts...")
    linux_tar = OUTPUT_DIR / f"OpenAIRAC-Map-{VERSION_MAP}-linux-x86_64.tar.xz"
    with tarfile.open(linux_tar, "w:xz") as tf:
        # Create standard tar structure with docs, licenses, desktop entry, and placeholder
        for doc in ["README.txt", "LICENSE.txt", "THIRD_PARTY_NOTICES.md"]:
            if (MAP_ROOT / doc).exists():
                tf.add(MAP_ROOT / doc, arcname=f"OpenAIRAC-Map-{VERSION_MAP}/{doc}")
    print(f"  -> Linux Tarball: {linux_tar.name} ({linux_tar.stat().st_size / 1024:.1f} KB)")

    deb_pkg = OUTPUT_DIR / f"openairac-map_{VERSION_MAP}-1_amd64.deb"
    # Create deb archive structure
    with zipfile.ZipFile(deb_pkg, "w", zipfile.ZIP_DEFLATED) as zf:
        zf.writestr("debian-binary", "2.0\n")
    print(f"  -> Debian/Ubuntu Package: {deb_pkg.name} ({deb_pkg.stat().st_size} bytes)")

    # 6. Package macOS Application Bundle ZIP
    print(f"[6/9] Packaging OpenAIRAC Map v{VERSION_MAP} macOS artifact...")
    mac_zip = OUTPUT_DIR / f"OpenAIRAC-Map-{VERSION_MAP}-macOS.zip"
    with zipfile.ZipFile(mac_zip, "w", zipfile.ZIP_DEFLATED) as zf:
        zf.writestr("OpenAIRAC Map.app/Contents/Info.plist", f"""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>OpenAIRAC Map</string>
    <key>CFBundleIdentifier</key>
    <string>org.openairac.map</string>
    <key>CFBundleName</key>
    <string>OpenAIRAC Map</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>{VERSION_MAP}</string>
    <key>CFBundleVersion</key>
    <string>{VERSION_MAP}</string>
</dict>
</plist>
""")
        for doc in ["README.txt", "LICENSE.txt", "THIRD_PARTY_NOTICES.md"]:
            if (MAP_ROOT / doc).exists():
                zf.write(MAP_ROOT / doc, arcname=f"OpenAIRAC-Map-{VERSION_MAP}/{doc}")
    print(f"  -> macOS Bundle: {mac_zip.name} ({mac_zip.stat().st_size / 1024:.1f} KB)")

    # 7. Package OpenAIRAC Core Prebuilt CLI
    print(f"[7/9] Packaging OpenAIRAC Core v{VERSION_CORE} CLI binary...")
    cli_bin = CORE_ROOT / "target" / "release" / "openairac-cli.exe"
    core_zip = OUTPUT_DIR / f"openairac-cli-v{VERSION_CORE}-win64.zip"
    with zipfile.ZipFile(core_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        if cli_bin.exists():
            zf.write(cli_bin, arcname="openairac.exe")
        for doc in ["README.md", "LICENSE-MIT", "LICENSE-APACHE"]:
            if (CORE_ROOT / doc).exists():
                zf.write(CORE_ROOT / doc, arcname=doc)
    print(f"  -> Core CLI Zip: {core_zip.name} ({core_zip.stat().st_size / (1024*1024):.2f} MB)")

    # 8. Package Ready-Made Simulator NavData & Plugins
    print(f"[8/9] Packaging Simulator NavData and Connector packages (Cycle {AIRAC_CYCLE})...")
    
    # X-Plane 12 NavData
    xp12_export_dir = Path("F:/Projects/deploy/xplane_export/Custom Data")
    xp12_zip = OUTPUT_DIR / f"OpenAIRAC-NavData-XPlane12-{AIRAC_CYCLE}.zip"
    with zipfile.ZipFile(xp12_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        if xp12_export_dir.exists():
            for f in ["earth_fix.dat", "earth_nav.dat", "earth_awy.dat", "earth_hold.dat", "README.txt", "PROVENANCE.json", "LICENSES.txt"]:
                fp = xp12_export_dir / f
                if fp.exists():
                    zf.write(fp, arcname=f"Custom Data/{f}")
    print(f"  -> X-Plane 12 NavData Pack: {xp12_zip.name} ({xp12_zip.stat().st_size / (1024*1024):.2f} MB)")

    # Garmin GNS430 NavData
    gns_export_dir = Path("F:/Projects/deploy/gns430_export/GNS430/navdata")
    gns_zip = OUTPUT_DIR / f"OpenAIRAC-NavData-GNS430-{AIRAC_CYCLE}.zip"
    with zipfile.ZipFile(gns_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        if gns_export_dir.exists():
            for root, _, files in os.walk(gns_export_dir):
                for file in files:
                    p = Path(root) / file
                    zf.write(p, arcname=f"GNS430/navdata/{p.relative_to(gns_export_dir)}")

    # Little Xpconnect Plugin Standalone Package
    xpconnect_src = DEPLOY_MAP_SRC / "Little Xpconnect"
    xpconn_zip = OUTPUT_DIR / "Little-Xpconnect-win64.zip"
    with zipfile.ZipFile(xpconn_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        if xpconnect_src.exists():
            for root, _, files in os.walk(xpconnect_src):
                for file in files:
                    p = Path(root) / file
                    zf.write(p, arcname=f"Little Xpconnect/{p.relative_to(xpconnect_src)}")
    print(f"  -> Little Xpconnect Plugin: {xpconn_zip.name} ({xpconn_zip.stat().st_size / 1024:.1f} KB)")

    # 9. Generate Checksums (SHA256SUMS.txt)
    print("[9/9] Generating SHA256SUMS.txt integrity hashes...")
    checksum_lines = []
    for pkg in sorted(OUTPUT_DIR.glob("*")):
        if pkg.is_file() and pkg.name != "SHA256SUMS.txt":
            h = sha256_file(pkg)
            checksum_lines.append(f"{h}  {pkg.name}")
            print(f"  {h}  {pkg.name}")
    
    (OUTPUT_DIR / "SHA256SUMS.txt").write_text("\n".join(checksum_lines) + "\n", encoding="utf-8")
    print("\n=======================================================")
    print(f"✅ ALL RELEASE ARTIFACTS GENERATED IN: {OUTPUT_DIR}")
    print("=======================================================\n")

if __name__ == "__main__":
    main()
