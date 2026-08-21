# X-Plane 12 Navigation Data: Production Strategy

Status: decision record for OpenAIRAC v0.3 (routing & procedures foundation).

## The two delivery paths

X-Plane 12 consumes navigation data in two ways:

| Path | Format | Producer | Used for |
|---|---|---|---|
| **A. `earth_424.dat` (Custom Data)** | ARINC 424 flight-plan section (predecessor of CIFP) | Third-party converter | FMC procedures (SID/STAR/approach legs) |
| **B. XPNAV1200 / XPFIX1200 / XPAWY1101 data files** | Laminar legacy per-class text formats | Any tool | Navaids, fixes, airway edges |

Path B is what the OpenAIRAC native exporter (`openairac-export-xplane`)
writes today. Path A is what a real aircraft FMC needs for terminal
procedures.

## Decision: use convert424toxplane for production

The **production path for shipping navigraph-style data to X-Plane is
convert424toxplane (v12.4), running on a genuine ARINC 424 file**. The
native exporter is a diagnostic/validation tool, not the shipping
format.

Rationale:

1. **Scope.** Reimplementing the full `earth_424.dat` FMC record set
   (PD/PE/PF procedure legs, runway thresholds, terminal waypoints,
   ARINC 424 alignment) is a multi-month effort against an
   undocumented, reverse-engineered format. convert424toxplane is the
   community reference implementation, actively maintained, and its
   output is what X-Plane users install today.
2. **Fidelity risk.** The FMC section is safety-adjacent: a subtly
   wrong procedure leg renders approaches unflyable. Hand-written
   exporters must be golden-verified record-by-record (see below for
   what that costs).
3. **OpenAIRAC's actual product value is upstream**: a verified,
   temporal, fail-closed data foundation (FAA CIFP → canonical store →
   queries/routing/validation). Feeding the store into a proven
   converter is the boring, correct choice.
4. **Input format.** convert424toxplane expects a plain-text ARINC 424
   file (column layout compatible with FAA CIFP records). The FAA CIFP
   (cycle 2608, `CIFP_260806.zip`) is such a file; converter output
   for KSFO was verified byte-for-byte against OpenAIRAC's decoded
   entities during v0.3 development.

## What the native exporter is for

`openairac-export-xplane` remains the **validation/diagnostic pair** of
the canonical store:

* Referential integrity on actually-serialized rows
  (`ExportedEntityIndex`).
* Physical row counts in `manifest.json`.
* Staged-but-sequential swap (never a torn half-written dataset).
* Golden tests against official Laminar spec examples (XPNAV1200
  Seattle row, XPAWY1101 type table 11/2/3).

It is not the FMC path. That division is deliberate and documented so
nobody re-derives the ARINC 424 FMC record set from scratch.

## Pipeline

```
FAA CIFP (ARINC 424 plain text)
        │ openairac-ingest (layered decoder, verified column map)
        ▼
canonical temporal store (SQLite, revisioned)
        │ WorldQuery (openairac-service)
        ▼
   three consumers:
   1. Flight planning (routing + procedures + integration)
   2. Validation: native exporter diagnostics (XPNAV/XPFIX/XPAWY)
   3. Production export: CIFP → convert424toxplane → earth_424.dat
```

Note: convert424toxplane consumes the source CIFP directly; the store
is the *authoritative check* against its output, not its input
transformer. When a future OpenAIRAC milestone adds a native FMC
writer, it will export from the canonical store with
record-by-record golden verification against converter output —
the same cross-checking discipline used to decode PD/PE/PF/PC in v0.3.

## Verification discipline (how v0.3 decoded records)

For every record class the project decodes:

1. Download the official cycle (FAA CIFP 2608) and the reference
   converter (convert424toxplane v12.4, incl. `geoids/`).
2. Generate per-airport reference output (`CIFP/KSFO.dat`).
3. Decode raw records with the layered decoder; diff field-by-field
   against the converter's interpretation.
4. Freeze verified column maps as golden tests with real records.

No field is ever guessed: unsupported or unidentified columns are
preserved verbatim in `raw` and reported as unsupported.


## Golden compatibility harness (v0.7)

`crates/openairac-export-xplane/examples/golden_compat.rs` runs
convert424toxplane v12.4 on a CIFP master file and diffs its
earth_fix/earth_nav/earth_awy output against the OpenAIRAC exporter
fed from the SAME CIFP through the canonical store. Manual tool;
requires the converter binary (never runs in CI).

Cycle 2608 run results (2026-08-16):

- **earth_fix.dat**: every one of our 32,431 rows is value-identical
  to the converter (whitespace-normalized). Zero fabricated rows;
  the 37,630 converter-only rows are records we deliberately skip
  because required fields are missing (fail-closed; the converter
  defaults them).
- **earth_nav.dat**: 2,694 shared component rows; 2,604 differ only
  in name cosmetics (the converter strips redundant `NAR` prefixes
  and defaults unknown fields; ours writes source values verbatim).
  Residual class diffs: 8 standalone-DME facilities with `U`
  (undetermined) class where the converter picks 150/125 with no
  published discriminator — ours uses a documented deterministic 125.
- **earth_awy.dat**: 13,106 shared first-segment pairs, 12,332
  value-identical; the rest are converter chain-row consolidations
  and converter defaulting unknown MEAs to 0 (we skip or use real
  values).

### Real-data fixes the harness found

1. **Airway MEA off-by-one** (fixed): the MEA published on a fix
   record applies to the segment FOLLOWING that fix. The decoder
   attached it to the segment ending at the fix. Verified: V257
   AADCO->VERNE now carries FL115, matching the converter.
2. **D-record elevation/magvar columns** (fixed): elevation is cols
   80-84 in whole feet (was 81-85 read as tenths — DBL's 11,800 ft
   decoded as 1,800); magnetic variation is cols 75-79 (was 75-80 —
   12.0E read as 12.01E). Verified against DBL/CDO/ISFO records.
3. **XPAWY merge semantics** (fixed): airways sharing a segment with
   DIFFERENT altitude bands now emit separate rows (the band on a
   merged row is undefined per XPAWY1101); CIFP records without an
   H/L marker (oceanic/other routes) emit as low, matching the
   converter.
4. **DME service class** (fixed): VOR-family `U`-class records map to
   150, standalone DME `U`-class to 125 — verified against converter
   output for CDO/ADK/EEA.


## Procedure golden harness (RC)

`crates/openairac-export-xplane/examples/golden_procedures.rs`
compares every decoded procedure leg chain (fix sequence + path
terminator per procedure/transition) against the converter's
per-airport `CIFP/<airport>.dat` files.

Cycle 2608 result: **1,379/1,379 chains identical** across
KSFO (180), KDEN (467), KJFK (83), KLAX (400), KORD (249) — zero
differences in either direction. Coverage: SIDs, STARs, ILS
approaches (I-series), localizer-only (L-series), RNAV (H-series),
transitions, and missed-approach legs (FM/HA/HM terminators).


## LPV / SBAS strategy (stage 0 decision)

Research basis: official Laminar specs (XP-CIFP1250/1101) and the
X-Plane developer documentation.

1. **The official deployment path for terminal procedures is the
   converter-generated per-airport `CIFP/$ICAO.dat` set** (XP CIFP
   1250), produced by Laminar's own convert424toxplane tool. The spec
   explicitly states these files must never be hand-built: the
   converter is the only sanctioned encoder.
2. **The raw `FAACIFP18` rename hack (`earth_424.dat`) is NOT
   officially supported** and causes simulator instability — rejected.
3. **LPV/SBAS needs no special rows.** The CIFP1250 row set is
   SID/STAR/APPCH/RWY/PRDAT (+ helicopter variants in 12.5). LPV
   vertical guidance travels in the PF legs themselves: the ARINC
   5.70 vertical angle and 5.211 RNP fields on the final segment of
   RNAV (R-series) approaches. OpenAIRAC decodes both (verified on
   cycle 2608: KSFO R10L legs carry -3.0 deg / RNP 0.31, R19RY
   -3.15 / 0.31).
4. **Decision: FAA CIFP -> convert424toxplane -> Custom Data/CIFP/**
   remains the production US terminal-data strategy** (already
   documented above). No PP->row-14/16 reimplementation; the
   earth_nav.dat SBAS point rows 14/16 are converter-synthesized
   display geometry, not a prerequisite for LPV guidance.
5. **Simulator-side requirement (documented, not ours):** the flown
   aircraft must have an SBAS receiver assigned in Plane Maker
   (Systems > Bus Loads) for the LPV glideslope to appear — a
   navdata-independent aircraft configuration issue.

Cycle compatibility: the converter consumed cycles 2608 and 2609
successfully in our harnesses; current convert424toxplane versions
support the current FAA CIFP layout (see the X-Plane forum thread on
recent CIFP downloads for version requirements).
## Related

* `crates/openairac-ingest/src/faa_cifp.rs` — layered CIFP decoder
  (EA/D/DB/PN/ER + PA/PG/PC/PD/PE/PF, cycle 2608 verified).
* `crates/openairac-export-xplane` — native XPNAV/XPFIX/XPAWY exporter
  (diagnostic path).
* `docs/ARCHITECTURE.md` — crate map and boundaries.
