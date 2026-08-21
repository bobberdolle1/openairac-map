# OpenAIRAC Map Integration Guide

This document describes the relationship between the OpenAIRAC core compiler and the OpenAIRAC Map GUI flight planning client (`bobberdolle1/openairac-map`).

---

## 1. Overview

OpenAIRAC Map is the official GUI and flight planning moving-map application for the OpenAIRAC ecosystem, based on the mature, GPL-licensed Little Navmap codebase.

OpenAIRAC core produces SQLite navigation databases directly consumable by OpenAIRAC Map:
- **Format Family**: `little-navmap-sqlite`
- **Output Artifact**: `openairac.sqlite` (and backward-compatible `little_navmap_openairac.db`)
- **Schema Contract**: atools / Little Navmap v14.29 database schema

---

## 2. Generating Databases for OpenAIRAC Map

```bash
# Export OpenAIRAC database for OpenAIRAC Map
openairac export lnm --out ./dist/openairac-map

# Or using the direct exporter:
cargo run -p openairac-export-lnm --example lnm_export -- <world.sqlite> <output_dir> [effective_date]
```

### Installation Targets

| Platform | Target Database Path |
|---|---|
| Windows | `%APPDATA%\ABarthel\little_navmap_db\openairac.sqlite` |
| Linux | `~/.config/ABarthel/little_navmap_db/openairac.sqlite` |
| macOS | `~/Library/Application Support/ABarthel/little_navmap_db/openairac.sqlite` |

---

## 3. Product Principles

1. **OpenAIRAC comes first**: OpenAIRAC is the default and preferred navigation provider in OpenAIRAC Map.
2. **Strict Database Separation**: OpenAIRAC and optional third-party providers (e.g. Navigraph) use separate SQLite files (`openairac.sqlite` vs `little_navmap_navigraph.sqlite`).
3. **No Silent Fallback**: If an open source dataset does not contain terminal procedures, OpenAIRAC Map honestly reports "Not available in OpenAIRAC" rather than silently substituting proprietary data.
4. **Scenery Blending**: OpenAIRAC navigation data seamlessly blends with simulator scenery layout for airport taxiways, gates, and 3D terrain.
