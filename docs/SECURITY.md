# OpenAIRAC Release Signing

Bundles are Ed25519-signed. This document defines the operational
provisioning workflow for production signing: offline private key,
public trust root, key id, rotation, revocation, and release signing.

## Threat model

A signed bundle authenticates that the bundle was produced by a holder
of the signing private key. Verification is fail-closed:

- `SignedTrusted` bundles without a configured trust root are REJECTED.
- A signature that does not verify against the configured trust root
  is REJECTED.
- An unsigned bundle carrying a stray signature file is REJECTED.
- A `SignedTrusted` marker with no `manifest.sig` is REJECTED.

OpenAIRAC trusts only keys the operator explicitly supplies. There is
no built-in root of trust, no key distribution server, and no TOFU.

## Key material

- **Private key**: 32-byte Ed25519 seed, base64, one line. SECRET.
  Never commit it, never email it, never put it in CI, never ship it
  in a bundle.
- **Public trust root**: 32-byte Ed25519 public key, base64, one line.
  Published with the release channel.
- **Key id**: `sha256(public_key)[..16]` hex. Used for rotation
  bookkeeping and audit logs.

## Provisioning workflow (offline)

1. On an OFFLINE machine (no network, full-disk encryption):

   ```
   openairac keygen generate \
     --private-key openairac-signing.key \
     --public-key  openairac-signing.pub
   ```

   This prints the key id. Record it in the release log.

2. Move the private key to offline storage (hardware token or an
   encrypted volume on the offline machine). The private key never
   leaves that machine except via the signing step below.

3. Publish `openairac-signing.pub` (public key + key id) in the
   release channel metadata. This file is NOT secret.

4. Only the public key is ever committed to any repository.

## Signing a release

On the offline machine (the bundle directory is transferred by
sneakernet or generated there):

```
openairac bundle sign \
  --bundle <bundle-dir> \
  --private-key openairac-signing.key
```

This flips the bundle's authenticity to `SignedTrusted`, recomputes
the content hash, and writes `manifest.sig`. Signing is the ONLY
operation the private key is used for.

## Verification on operator machines

```
openairac bundle verify --bundle <bundle-dir> --trust openairac-signing.pub
openairac bundle install --bundle <bundle-dir> --root <root> --trust openairac-signing.pub
```

## Key rotation

Rotation = publishing a new public key alongside the old one during a
rollover window. `--trust` accepts multiple files; verification
succeeds against ANY supplied root. Procedure:

1. Generate the successor key offline (same workflow as above).
2. Publish the successor public key in the channel.
3. Sign new releases with the successor key.
4. After the last old-key-signed bundle leaves support, remove the old
   public key from the channel documentation.

The key id distinguishes which key signed a given release in audit
trails: `openairac keygen id --public-key <key>`.

## Revocation (compromise response)

1. Remove the compromised public key from the published trust set
   immediately. Operators who still configure it are on their own —
   the channel documentation is the revocation list.
2. Generate a replacement key offline.
3. Re-sign and re-publish every bundle still in support with the
   replacement key (do NOT re-publish old bundles signed only by the
   compromised key without re-signing).
4. Record the incident: key id, date, scope of compromise.

Revocation is procedural, not cryptographic: OpenAIRAC has no online
key distribution; the published trust set IS the policy.

## Development keys

Test keys generated during development (CI, examples, tests) are
ephemeral and MUST NOT be used to sign releases. The production
private key is provisioned exactly once by the workflow above.

## Repository policy

- The production private key is NEVER committed (`.gitignore` guards
  `*.key`).
- CI has no signing capability; release signing is an offline,
  human-triggered step.
- Bundles built during development remain `UnsignedDevelopment`; they
  install without trust configuration by design and are NOT for
  distribution to third parties.
