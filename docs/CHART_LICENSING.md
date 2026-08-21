# OpenAIRAC Chart Licensing & Redistribution Policy

This document defines the 4-tier licensing classification system applied to aeronautical charts and assets in OpenAIRAC.

---

## 1. The 4-Tier Policy Model

Every chart document and asset in OpenAIRAC is governed by `openairac_model::RedistributionPermission`:

```text
┌─────────────────────────────────────────────────────────────┐
│              OpenAIRAC Chart Policy Hierarchy               │
├─────────────────────────────┬───────────────────────────────┤
│ Tier 1: PublicRedistribution│ Fully open/public domain data │
│                             │ (FAA d-TPP, Etalab v2.0)      │
├─────────────────────────────┼───────────────────────────────┤
│ Tier 2: LocalOnly           │ User-supplied or account-only │
│                             │ datasets (DFS AIS, local AIP) │
├─────────────────────────────┼───────────────────────────────┤
│ Tier 3: MetadataOnly        │ Frequencies & chart titles    │
│                             │ without bundled PDF payload   │
├─────────────────────────────┼───────────────────────────────┤
│ Tier 4: Forbidden           │ Proprietary/encrypted bundles │
│                             │ (Navigraph/Jeppesen scraped)  │
└─────────────────────────────┴───────────────────────────────┘
```

---

## 2. Asset vs. Metadata Separation

- **Public Metadata**: Chart indexes, document identifiers, titles, cycles, and procedure associations can be synchronized and shared under open licenses.
- **Asset Redistribution**: Chart PDF assets are only bundled or mirrored publicly if the publisher explicitly grants redistribution rights (`PublicRedistribution`).
- **Local Caching**: When a user on-demand downloads an asset from an official government portal, the asset is stored in the user's private local cache (`charts_cache/`).

---

## 3. Strict Prohibitions

1. **No Proprietary Scraping**: OpenAIRAC never reverse-engineers or scrapes commercial chart platforms (Navigraph, Jeppesen, Lido).
2. **No License Contamination**: Proprietary chart files are never committed to repository fixtures or distributed in public OpenAIRAC bundles.
3. **No Masquerading**: Government charts retain their authentic publisher branding and attribution.
