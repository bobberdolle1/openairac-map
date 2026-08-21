#!/usr/bin/env python3
"""
OpenAIRAC Map — Automated Windows Release Packaging Pipeline

Builds:
1. Portable ZIP: OpenAIRAC-Map-2.3.0-win64.zip
2. Standalone Windows GUI Setup: OpenAIRAC-Map-2.3.0-Setup.exe
3. Integrity Hashes: SHA256SUMS.txt

Enforces restricted payload leak scanning before any package is produced.
"""

import sys
import os
import shutil
import subprocess
import hashlib
import zipfile
from pathlib import Path

CSC_PATH = Path(r"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\csc.exe")
MAP_ROOT = Path("F:/Projects/openairac-map")
BUILD_DIR = Path("F:/Projects/build-openairac-map-release")
DEPLOY_SRC = Path("F:/Projects/deploy/Little Navmap win64")
OUTPUT_DIR = Path("F:/Projects/deploy/release_v230")
VERSION = "2.3.0"

def sha256_file(filepath: Path) -> str:
    h = hashlib.sha256()
    with open(filepath, "rb") as f:
        while chunk := f.read(65536):
            h.update(chunk)
    return h.hexdigest()

def main():
    print(f"=== OpenAIRAC Map v{VERSION} Release Packaging Pipeline ===")
    
    if not CSC_PATH.exists():
        print(f"ERROR: C# compiler not found at {CSC_PATH}")
        sys.exit(1)

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    # 1. Sync latest binary from build directory
    built_exe = BUILD_DIR / "littlenavmap.exe"
    if built_exe.exists():
        print(f"[1/7] Copying latest built executable from {built_exe}...")
        shutil.copy2(built_exe, DEPLOY_SRC / "littlenavmap.exe")
    else:
        print(f"WARNING: {built_exe} not found, using existing deployed binary.")

    # 2. Sync metadata and documentation
    print("[2/7] Updating release metadata and notices...")
    shutil.copy2(MAP_ROOT / "README.txt", DEPLOY_SRC / "README.txt")
    shutil.copy2(MAP_ROOT / "LICENSE.txt", DEPLOY_SRC / "LICENSE.txt")
    if (MAP_ROOT / "THIRD_PARTY_NOTICES.md").exists():
        shutil.copy2(MAP_ROOT / "THIRD_PARTY_NOTICES.md", DEPLOY_SRC / "THIRD_PARTY_NOTICES.md")
    
    (DEPLOY_SRC / "version.txt").write_text(f"win64-{VERSION}\n", encoding="utf-8")

    # 3. Compile Uninstaller into deployment staging
    print("[3/7] Compiling standalone Uninstaller (Uninstall.exe)...")
    uninstaller_cs = MAP_ROOT / "tools" / "uninstaller_source.cs"
    uninstaller_exe = DEPLOY_SRC / "Uninstall.exe"
    
    cmd_uninst = [
        str(CSC_PATH),
        "/target:winexe",
        "/optimize+",
        "/platform:x64",
        "/reference:System.dll",
        "/reference:System.Windows.Forms.dll",
        "/reference:System.Drawing.dll",
        f"/out:{uninstaller_exe}",
        str(uninstaller_cs)
    ]
    res_uninst = subprocess.run(cmd_uninst, capture_output=True, text=True)
    if res_uninst.returncode != 0:
        print(f"ERROR compiling uninstaller:\n{res_uninst.stdout}\n{res_uninst.stderr}")
        sys.exit(1)

    # 4. Run Restricted Payload Leak Scanner Gate
    print("[4/7] Running Restricted Payload Leak Scanner...")
    scan_script = MAP_ROOT / "tools" / "scan_leaks.py"
    res_scan = subprocess.run([sys.executable, str(scan_script), str(DEPLOY_SRC)], capture_output=True, text=True)
    print(res_scan.stdout)
    if res_scan.returncode != 0:
        print(res_scan.stderr)
        print("❌ LEAK SCAN FAILED! Aborting release packaging.")
        sys.exit(1)

    # 5. Create Portable ZIP
    zip_filename = f"OpenAIRAC-Map-{VERSION}-win64.zip"
    zip_out = OUTPUT_DIR / zip_filename
    print(f"[5/7] Creating Portable ZIP archive: {zip_out}...")
    
    with zipfile.ZipFile(zip_out, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zipf:
        for root, dirs, files in os.walk(DEPLOY_SRC):
            for file in files:
                fpath = Path(root) / file
                arcname = Path(f"OpenAIRAC-Map-{VERSION}") / fpath.relative_to(DEPLOY_SRC)
                zipf.write(fpath, arcname)

    print(f"  -> Portable ZIP size: {zip_out.stat().st_size / (1024*1024):.2f} MB")

    # 6. Compile Standalone GUI Installer (with embedded payload.zip)
    print("[6/7] Compiling Windows GUI Installer (OpenAIRAC-Map-2.3.0-Setup.exe)...")
    
    # Create internal payload zip for the installer (without root folder prefix)
    installer_payload_zip = OUTPUT_DIR / "temp_installer_payload.zip"
    with zipfile.ZipFile(installer_payload_zip, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as zipf:
        for root, dirs, files in os.walk(DEPLOY_SRC):
            for file in files:
                fpath = Path(root) / file
                arcname = fpath.relative_to(DEPLOY_SRC)
                zipf.write(fpath, arcname)

    installer_cs = MAP_ROOT / "tools" / "installer_source.cs"
    installer_exe = OUTPUT_DIR / f"OpenAIRAC-Map-{VERSION}-Setup.exe"

    cmd_inst = [
        str(CSC_PATH),
        "/target:winexe",
        "/optimize+",
        "/platform:x64",
        "/reference:System.dll",
        "/reference:System.Windows.Forms.dll",
        "/reference:System.Drawing.dll",
        "/reference:System.IO.Compression.dll",
        "/reference:System.IO.Compression.FileSystem.dll",
        f"/resource:{installer_payload_zip},payload.zip",
        f"/out:{installer_exe}",
        str(installer_cs)
    ]
    res_inst = subprocess.run(cmd_inst, capture_output=True, text=True)
    
    # Clean up temp payload zip
    if installer_payload_zip.exists():
        installer_payload_zip.unlink()

    if res_inst.returncode != 0:
        print(f"ERROR compiling installer:\n{res_inst.stdout}\n{res_inst.stderr}")
        sys.exit(1)

    print(f"  -> Installer size: {installer_exe.stat().st_size / (1024*1024):.2f} MB")

    # 7. Generate SHA256SUMS.txt
    print("[7/7] Generating SHA256SUMS.txt integrity hashes...")
    sha_lines = []
    
    setup_hash = sha256_file(installer_exe)
    zip_hash = sha256_file(zip_out)
    
    sha_lines.append(f"{setup_hash}  {installer_exe.name}")
    sha_lines.append(f"{zip_hash}  {zip_out.name}")
    
    sha_file = OUTPUT_DIR / "SHA256SUMS.txt"
    sha_file.write_text("\n".join(sha_lines) + "\n", encoding="utf-8")
    
    print("\n=======================================================")
    print("✅ RELEASE PACKAGING COMPLETED SUCCESSFULLY!")
    print(f"Destination: {OUTPUT_DIR}")
    print(f"  1. Installer: {installer_exe.name} ({installer_exe.stat().st_size / (1024*1024):.2f} MB)")
    print(f"     SHA-256: {setup_hash}")
    print(f"  2. Portable:  {zip_out.name} ({zip_out.stat().st_size / (1024*1024):.2f} MB)")
    print(f"     SHA-256: {zip_hash}")
    print(f"  3. Checksums: {sha_file.name}")
    print("=======================================================\n")

if __name__ == "__main__":
    main()
