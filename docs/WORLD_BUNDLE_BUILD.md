# OpenAIRAC World Bundle Build & Manifest Specification

## 1. Composition CLI Commands

```bash
# Inspect the active world-open provider composition and licenses
openairac bundle explain --json

# Fuse authoritative national and global baseline data into world database
openairac bundle compose-world-open --json

# Build signed deterministic world-open distribution bundle
openairac bundle build --db ./data/world.openairac.sqlite --out ./bundles
```

## 2. WorldOpenManifest Structure
Every world-open release bundle includes a `manifest.json` recording:
- Target AIRAC cycle (e.g. `2608` / `2609`)
- Included provider identities and dataset formats
- Snapshot retrieval timestamps and content SHA-256 hashes
- Machine-readable country coverage metrics
- Cryptographic Ed25519 digital signature
