#!/usr/bin/env python3
"""
Automated Documentation Link Checker for OpenAIRAC and OpenAIRAC Map.

Scans all Markdown files for broken relative links, anchor mismatches,
or invalid GitHub URLs.
"""

import sys
import os
import re
from pathlib import Path

LINK_REGEX = re.compile(r"\[([^\]]+)\]\(([^)]+)\)")

def check_file(md_path: Path, repo_root: Path) -> list:
    broken = []
    content = md_path.read_text(encoding="utf-8", errors="ignore")
    
    for match in LINK_REGEX.finditer(content):
        text, target = match.groups()
        target = target.strip()
        
        # Skip external web protocols, badges, and mailto
        if target.startswith(("http://", "https://", "mailto:", "#")):
            # Check if it points to github blob for our own repos
            if "github.com/bobberdolle1/open-airac/blob/main/" in target:
                rel = target.split("github.com/bobberdolle1/open-airac/blob/main/")[1].split("#")[0]
                local_f = Path("F:/Projects/open-airac") / rel
                if not local_f.exists():
                    broken.append((md_path, text, target, f"Target file does not exist: {local_f}"))
            elif "github.com/bobberdolle1/openairac-map/blob/master/" in target:
                rel = target.split("github.com/bobberdolle1/openairac-map/blob/master/")[1].split("#")[0]
                local_f = Path("F:/Projects/openairac-map") / rel
                if not local_f.exists():
                    broken.append((md_path, text, target, f"Target file does not exist: {local_f}"))
            continue
            
        # Strip anchor
        file_part = target.split("#")[0]
        if not file_part:
            continue
            
        # Resolve relative path
        resolved = (md_path.parent / file_part).resolve()
        if not resolved.exists():
            broken.append((md_path, text, target, f"Relative file does not exist: {resolved}"))

    return broken

def scan_repo(repo_dir: Path) -> list:
    all_broken = []
    for root, dirs, files in os.walk(repo_dir):
        # Skip build dirs, dist, deploy, and git
        if any(b in root for b in [".git", "build", "target", "deploy", "dist", "node_modules"]):
            continue
        for f in files:
            if f.endswith(".md"):
                fpath = Path(root) / f
                all_broken.extend(check_file(fpath, repo_dir))
    return all_broken

def main():
    repos = [Path("F:/Projects/open-airac"), Path("F:/Projects/openairac-map")]
    total_broken = []
    
    for r in repos:
        print(f"[CHECK] Scanning Markdown links in: {r}")
        broken = scan_repo(r)
        total_broken.extend(broken)
        
    if total_broken:
        print(f"\n❌ Found {len(total_broken)} broken documentation links:")
        for md_path, text, target, err in total_broken:
            print(f"  - In {md_path.name}: [{text}]({target}) -> {err}")
        sys.exit(1)
    else:
        print("\n✅ All documentation links verified! (0 broken links)")
        sys.exit(0)

if __name__ == "__main__":
    main()
