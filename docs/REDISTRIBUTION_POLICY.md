# OpenAIRAC Data Redistribution and Licensing Policy

## 1. Legal and Ethical Commitment

OpenAIRAC is dedicated to building an open, clean-room, worldwide aeronautical data ecosystem for flight simulation. We strictly respect the intellectual property and licensing terms of government civil aviation authorities, open data initiatives, and proprietary providers.

**Core Rules:**
1. **Never scrape behind paywalls or authentication without clear authorization.**
2. **Never redistribute datasets whose redistribution rights are not explicitly granted.**
3. **Never ingest, store, or distribute proprietary commercial navdata (Navigraph, Jeppesen, NavDataPro).**
4. **Isolate user BYOD (Bring-Your-Own-Data) to local compilation environments.**

## 2. Four-Tier Redistribution Policy

OpenAIRAC models all data sources through the strongly-typed `RedistributionPermission` enumeration:

```text
                                  +------------------------------------+
                                  | Aeronautical Data Ingestion Source |
                                  +------------------------------------+
                                                     │
                   ┌─────────────────────────────────┼─────────────────────────────────┐
                   ▼                                 ▼                                 ▼
         [PublicRedistribution]                 [LocalOnly]                       [Forbidden]
                   │                                 │                                 │
                   ▼                                 ▼                                 ▼
   Included in Official Release Bundles     Personal Local Compiler Use      Rejected Fail-Closed:
   (FAA CIFP, OurAirports, Open Data)      (BYOD, User EAD Accounts)         (Navigraph, Jeppesen)
```

### Tier 1: `PublicRedistribution`
- **Definition**: Datasets published under official open data licenses, public domain mandates, or CC-BY/CC0 terms.
- **Usage**: Permitted for inclusion in official OpenAIRAC release bundles and signed distribution channels.
- **Examples**:
  - FAA CIFP & NASR AIXM (US Government Public Domain)
  - OurAirports (Dedicated to Public Domain via CC0 1.0)
  - OpenFlightmaps (Open aeronautical data)
  - DFS German AIP Open Data (GeoNutzV)

### Tier 2: `LocalOnly`
- **Definition**: Datasets legally acquired by an end user for personal use, but whose redistribution to third parties is restricted by terms of service or national AIP policies.
- **Usage**: Permitted for local parsing and exporter installation on the user's workstation. Cryptographically and mechanically blocked from inclusion in official signed release bundles.
- **Examples**:
  - Eurocontrol EAD (requires individual registered user login)
  - National AIP downloads requiring user authentication
  - User BYOD files (`openairac import aixm <file>`)

### Tier 3: `MetadataOnly`
- **Definition**: Datasets where only high-level identifiers and public metadata (frequencies, runway names) may be published, but detailed geometric procedure profiles must remain local.

### Tier 4: `Forbidden`
- **Definition**: Proprietary commercial navdata packages or encrypted vendor formats.
- **Usage**: OpenAIRAC tools immediately reject ingestion, parsing, and bundle creation if a forbidden provider is detected.

## 3. Automated Policy Enforcement in Bundle Builder

When `openairac-bundle` builds a distribution package:
- In **Public Release Mode** (`BundleScope::PublicRelease`): The bundle builder executes `validate_bundle_distribution_policy(&providers)`. If any source snapshot originates from a `LocalOnly`, `MetadataOnly`, or `Forbidden` provider, bundle creation is aborted with a fatal error.
- In **Local Development Mode** (`BundleScope::LocalDevelopment`): Local-only records are permitted, but the bundle's manifest is stamped with `Authenticity::LocalDevelopment`. Signing with the OpenAIRAC production key is strictly blocked.
