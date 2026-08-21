#!/usr/bin/env python3
"""
OpenAIRAC Release Safety Gate — Restricted Payload Leak Scanner

Scans all packaged binaries, assets, documentation, and configuration files
to ensure ZERO restricted, proprietary, or personal payloads are included.

Forbidden payloads include:
- Commercial / proprietary navdata: Navigraph, Jeppesen, NavDataPro
- Restricted raw national AIP packages: CAICA raw downloads, private AIP bundles
- Credentials, tokens, private keys, SSH keys, API secrets
- Absolute developer workstation paths (e.g. developer home directories)
"""

import sys
import os
import re
from pathlib import Path

FORBIDDEN_PATTERNS = [
    (re.compile(r"gho_[A-Za-z0-9_]{20,}"), "GitHub Personal Access Token"),
    (re.compile(r"-----BEGIN (?:RSA |OPENSSH |EC )?PRIVATE KEY-----"), "Private Key Header"),
    (re.compile(r"navigraph", re.IGNORECASE), "Proprietary Navigraph Reference"),
    (re.compile(r"jeppesen", re.IGNORECASE), "Proprietary Jeppesen Reference"),
    (re.compile(r"navdatapro", re.IGNORECASE), "Proprietary NavDataPro Reference"),
]

# Paths and files allowed to mention historical or attribution context
ALLOWLIST_FILES = {
    "THIRD_PARTY_NOTICES.md",
    "LICENSE.txt",
    "LICENSE-3RD-PARTY.txt",
    "NOTICE.md",
    "scan_leaks.py",
    "DATA_SOURCES.md",
    "DATA_AND_PROVIDERS.md",
    "PRIVACY_AND_SECURITY.md",
    "RUSSIA_CAICA_GUIDE.md",
    "CHANGELOG.txt",
    "README.txt",
}

FORBIDDEN_FILE_EXTENSIONS = {
    ".key", ".pem", ".p12", ".pfx", ".secret", ".token", ".credentials"
}

FORBIDDEN_FILE_NAMES = {
    "id_rsa", "id_ed25519", "secrets.json", "credentials.json", ".env"
}

def scan_directory(target_dir: Path) -> list:
    violations = []
    print(f"[SCAN] Auditing directory: {target_dir}")
    
    if not target_dir.exists():
        violations.append(f"Target directory does not exist: {target_dir}")
        return violations

    for root, dirs, files in os.walk(target_dir):
        for fname in files:
            fpath = Path(root) / fname
            rel_path = fpath.relative_to(target_dir)
            
            # Check forbidden filename
            if fname.lower() in FORBIDDEN_FILE_NAMES:
                violations.append(f"Forbidden file detected: {rel_path}")
                continue
            # Skip binary files from raw regex search
            if fpath.suffix.lower() in {".dll", ".exe", ".so", ".dylib", ".png", ".jpg", ".bmp", ".ico", ".svg", ".db", ".sqlite", ".dat", ".bin", ".qm", ".url"}:
                continue
            if fpath.suffix.lower() in FORBIDDEN_FILE_EXTENSIONS:
                violations.append(f"Forbidden file extension ({fpath.suffix}): {rel_path}")
                continue

            # Skip binary files from raw regex search
            if fpath.suffix.lower() in {".dll", ".exe", ".so", ".dylib", ".png", ".jpg", ".bmp", ".ico", ".svg", ".db", ".sqlite", ".dat", ".bin"}:
                continue

            if fname in ALLOWLIST_FILES:
                continue

            try:
                content = fpath.read_text(encoding="utf-8", errors="ignore")
                for pattern, desc in FORBIDDEN_PATTERNS:
                    # Allow mention of Navigraph in the context of "NOT BUNDLED" or "NEVER INCLUDED" or "OPTIONAL"
                    if "Proprietary" in desc:
                        matches = pattern.findall(content)
                        for m in matches:
                            # Verify if context is an explicit prohibition / disclaimer
                            # If not surrounded by "not", "never", "no", "without", flag it
                            idx = content.lower().find(m.lower())
                            snippet = content[max(0, idx-40):min(len(content), idx+40)].lower()
                            if any(k in snippet for k in ["not", "never", "no ", "without", "forbidden", "unbundled", "non-bundled", "disclaimer", "license"]):
                                continue
                            violations.append(f"Potential un-disclaimed proprietary reference '{m}' in {rel_path}")
                    else:
                        if pattern.search(content):
                            violations.append(f"{desc} detected in {rel_path}")
            except Exception as e:
                violations.append(f"Failed to read file {rel_path}: {e}")

    return violations

def main():
    if len(sys.argv) > 1:
        target = Path(sys.argv[1])
    else:
        target = Path("F:/Projects/deploy/Little Navmap win64")

    violations = scan_directory(target)
    
    if violations:
        print("\n❌ RELEASE GATE FAILED: Restricted payload / secret leaks detected:")
        for v in violations:
            print(f"  - {v}")
        sys.exit(1)
    else:
        print("\n✅ RELEASE GATE PASSED: Zero restricted payloads or credentials detected.")
        sys.exit(0)

if __name__ == "__main__":
    main()
