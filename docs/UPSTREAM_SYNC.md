# OpenAIRAC Map — Upstream Synchronization Guide

This document defines the synchronization strategy and architectural boundaries between upstream **Little Navmap** (`albar965/littlenavmap`) and **OpenAIRAC Map** (`bobberdolle1/openairac-map`).

---

## 1. Repositories & Remotes

| Role | Repository URL | Purpose |
|---|---|---|
| **Origin** | `https://github.com/bobberdolle1/openairac-map.git` | Official OpenAIRAC Map repository |
| **Upstream** | `https://github.com/albar965/littlenavmap.git` | Upstream Little Navmap master |

### Recommended Remote Setup

```bash
git remote -v
# origin   https://github.com/bobberdolle1/openairac-map.git (fetch & push)
# upstream https://github.com/albar965/littlenavmap.git (fetch & push)
```

---

## 2. Fork Baseline

- **Initial Fork Point**: Commit `e925c1486f7657a4623dfca23c215d3c07f49201` (`Split mapflags include file`) on `master`.
- **Upstream Version Baseline**: Little Navmap 3.0.x / 3.1.x develop series.
- **OpenAIRAC Map Version**: 0.1.0.

---

## 3. Merge & Rebase Strategy

To keep upstream merges fast and conflict-free:

1. **Rebase/Merge Cadence**: Sync regularly from `upstream/master` on every upstream minor release or bi-weekly.
2. **Preference**: Use `git merge upstream/master` with explicit merge commits to preserve clear upstream history.
3. **Conflict Resolution Policy**:
   - Upstream flight planning, Marble map widget, SimConnect, and routing algorithm improvements MUST be accepted.
   - OpenAIRAC provider abstraction (`src/openairac/`), OpenAIRAC database slot, provenance formatting, and branding layers MUST be retained.

### Command Workflow

```bash
# Fetch latest upstream commits
git fetch upstream master

# Ensure local master is clean
git checkout master

# Merge upstream changes
git merge upstream/master -m "merge: sync with upstream littlenavmap $(date +%Y-%m-%d)"

# Verify build and tests
qmake littlenavmap.pro
make -j$(nproc)
```

---

## 4. Architectural Boundaries

To minimize merge friction:

### Kept Strictly Close to Upstream (Avoid Invasive Changes)
- `src/route/` (Flight planning, calculation, route commands)
- `src/query/` (Map and spatial queries)
- `src/connect/` (SimConnect, XpConnect, Little Navconnect clients)
- `src/perf/` (Aircraft performance models)
- `src/weather/` (NOAA, METAR, GRIB weather engines)
- `src/marble/` (Marble mapping widget interfaces)

### OpenAIRAC Extension & Customization Modules (Isolated)
- `src/openairac/` (NavigationProvider abstraction, ProvenanceManager, CoverageManager, OpenAiracDbManager)
- `src/db/databasemanager.cpp` (Additive: OpenAIRAC database file resolution & Navigation Data menu)
- `src/common/htmlinfobuilder.cpp` (Additive: Provenance & Coverage display blocks)
- `docs/` (OpenAIRAC documentation and integration contracts)

---
## 5. Strict Upstream Contribution & Write Policy

**UPSTREAM `albar965/littlenavmap` IS STRICTLY READ-ONLY FOR OPENAIRAC DEVELOPMENT.**

OpenAIRAC Map is intentionally an independent, standalone GPL-3.0 fork of Little Navmap with custom branding, EFB workspaces, and OpenAIRAC navigation engine integration. We are **NOT** attempting to upstream OpenAIRAC-specific features into Little Navmap.

### Inviolable Rules:
1. **FORBIDDEN WITHOUT EXPLICIT HUMAN DIRECTIVE**:
   - Never push branches to `upstream`.
   - Never open pull requests against `albar965/littlenavmap`.
   - Never comment on or modify upstream issue/PR trackers.
2. **AUTOMATION SAFETY**:
   - All pull requests, issues, and releases must explicitly specify `--repo bobberdolle1/openairac-map` (e.g. `gh pr create -R bobberdolle1/openairac-map ...`) to prevent `gh` from defaulting to the parent repository.
3. **UPSTREAM CONTRIBUTION MODE (Only if specifically requested by user)**:
   - Must be an isolated, generic bugfix.
   - Zero branding or OpenAIRAC-specific references.
   - Must be built and tested against official upstream toolchain (Qt 6.5+, MinGW/GCC or Clang, C++20).
   - Requires explicit human review before PR creation.

---

## 6. Summary Table

| Category | Policy |
|---|---|
| Upstream Remote | Strictly READ-ONLY (fetch & merge only) |
| Fork Remote (`origin`) | Primary development repository (`bobberdolle1/openairac-map`) |
| Upstream Pull Requests | Strictly FORBIDDEN unless explicitly directed by human maintainer |
| Navigation Engine | OpenAIRAC is default primary provider (`openairac.sqlite`) |
| Navigraph | Preserved as optional integration; fallback is OFF by default |
