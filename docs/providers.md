# OpenAIRAC Provider Platform & Local AIP Vault

## 1. Overview & Architecture

OpenAIRAC is an open, reproducible aeronautical data platform that ingests, reconciles, and compiles worldwide navigation data for flight simulation without relying on proprietary or pirated data.

The system decouples data acquisition from the canonical aviation model via the **Provider SDK**:

```
SOURCE (Official Open AIP / Local Vault / BYOD)
  │
  ▼
Provider Adapter (Acquisition & Source Snapshotting)
  │
  ▼
Raw Source Snapshot (Immutable, SHA-256 Verified)
  │
  ▼
Parser & Validator (Deterministic Semantic/Geometric Verification)
  │
  ▼
Canonical Provider Dataset (100% Fine-Grained Provenance)
  │
  ▼
Policy & Licensing Gate (PublicRedistribution / LocalOnly / MetadataOnly)
  │
  ▼
Canonical Merge Engine (Layered Precedence & Conflict Detection)
  │
  ├─► Localhost REST API / CLI (`openairac provider ...`)
  ├─► OpenAIRAC Map Data Manager
  ├─► Exporters (X-Plane CIFP, Little Navmap SQLite, MSFS BGL, PMDG)
  └─► Flight Planning & Routing Engine
```

---

## 2. Provider Policies & Legal Redistribution

OpenAIRAC enforces a fail-closed licensing policy across all compilation and distribution pipelines:

| Policy | Description | Public Bundle | Local Vault |
|---|---|:---:|:---:|
| `PublicRedistribution` | Officially open / public domain data (e.g. FAA CIFP, SIA France, OurAirports, open flightmaps). | **YES** | **YES** |
| `LocalOnly` | Official national publications authorized for local personal use (e.g. Russian CAICA, German DFS, Eurocontrol EAD). | **NO (FAIL-CLOSED)** | **YES** |
| `MetadataOnly` | Only high-level frequencies and identifiers may be published; geometry remains local. | **METADATA ONLY** | **YES** |
| `Forbidden` | Proprietary, contractually restricted, or encrypted datasets (e.g. Navigraph, Jeppesen, NavDataPro). | **FORBIDDEN** | **FORBIDDEN** |

---

## 3. Central Provider Registry (`ProviderRegistryV2`)

The built-in registry (`openairac_model::ProviderRegistryV2`) includes:

1. **`ourairports`**: Worldwide open aerodrome, runway, navaid, and frequency baseline (CC0 1.0 Universal / Public Domain). Priority: `10`.
2. **`faa_cifp`**: US Federal Aviation Administration complete terminal procedures, airways, fixes, and runways in ARINC 424 (Public Domain). Priority: `50`.
3. **`sia_france`**: French DGAC / SIA national aeronautical dataset in AIXM 4.5 and official structured procedure publications (Licence Ouverte 2.0). Priority: `60`.
4. **`ru_caica_local`**: Russian Federation CAICA official procedure coding collection and ATS Route Manual for local BYOD ingestion (Local Personal Use Only). Priority: `100`.
5. **`dfs_germany`**: German DFS official AIP IFR/VFR aeronautical data (Local Use Only). Priority: `90`.
6. **`openflightmaps`**: European VFR chart and airspace data in AIXM 4.5 (ODbL). Priority: `40`.
7. **`synthetic_test`**: Deterministic synthetic test fixture for SDK integration testing (Disabled by default in production). Priority: `5`.

---

## 4. Local AIP Vault V2 Workflow

The Local AIP Vault (`data/vault/`) allows users to import official data packages locally:

### Staging & Inspection:
```bash
openairac provider list
openairac provider show ru_caica_local
openairac provider status
```

### Local Import:
```bash
openairac provider import ru_caica_local /path/to/official/caica_source.zip
```

### Validation & Atomic Activation:
```bash
openairac provider validate ru_caica_local
openairac provider activate ru_caica_local
```

### Rollback:
```bash
openairac provider rollback ru_caica_local
```

---

## 5. Machine-Readable JSON & REST API

All provider commands support `--json` output for automation, EFB integration, and OpenAIRAC Map:

- `openairac provider list --json`
- `openairac provider status --json`
- `openairac provider show <id> --json`
- `openairac provider coverage <id> --json`

### REST Endpoints:
- `GET /api/openairac/v1/providers`: List all registered providers.
- `GET /api/openairac/v1/providers/{id}`: Detailed descriptor and capabilities.
- `GET /api/openairac/v1/providers/{id}/coverage`: Machine-readable coverage metrics.

---

## 6. Implementing a New Provider Adapter

To add a new aviation authority or dataset:

```rust
use openairac_ingest::provider::{CanonicalProviderDataset, ProviderAdapter, RawSourceSnapshot};
use openairac_ingest::validation::ProviderValidationReport;
use openairac_model::ProviderDescriptor;
use anyhow::Result;

pub struct CustomAipAdapter {
    descriptor: ProviderDescriptor,
}

impl ProviderAdapter for CustomAipAdapter {
    fn descriptor(&self) -> &ProviderDescriptor {
        &self.descriptor
    }

    fn acquire(&self, source_hint: Option<&str>) -> Result<RawSourceSnapshot> {
        // Fetch or read raw source bytes
        todo!()
    }

    fn parse(&self, snapshot: &RawSourceSnapshot) -> Result<CanonicalProviderDataset> {
        // Parse into canonical airports, runways, navaids, fixes, procedures, ATS routes
        todo!()
    }

    fn validate(&self, dataset: &CanonicalProviderDataset) -> Result<ProviderValidationReport> {
        // Run generic semantic and geometric validation
        let mut report = ProviderValidationReport::new(self.descriptor().name.clone(), dataset.version_tag.clone());
        // Add domain validations
        Ok(report)
    }
}
```
