# OpenAIRAC Reference Validation Policy & Differential Diagnostics

## 1. Navigraph Reference Policy
User-supplied proprietary datasets (e.g. Navigraph AIRAC cycles) may be used locally as an independent differential validation oracle and for compatibility verification.

### Inviolable Safeguards:
1. **Never a Public Source**: Navigraph data is NEVER ingested into canonical store releases or distributed in OpenAIRAC release bundles.
2. **Read-Only Local Diagnostics**: Diagnostic comparison commands (`openairac debug-compare-airport`) operate locally on user-specified file paths only.
3. **No Committed Artifacts**: No proprietary records, binaries, or fixtures are committed to repository history.
4. **Triangulation**: All discrepancies are triangulated against official government sources (FAA CIFP, SIA France, Eurocontrol, AIP).
