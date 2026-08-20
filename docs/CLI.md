# OpenAIRAC Map CLI (`openairac-map-cli`)

`openairac-map-cli` provides scriptable command-line automation for OpenAIRAC Map with full `--json` support.

## Usage Examples

```bash
# Status
openairac-map-cli status --json

# Aircraft-aware random flight plan (Boeing 747 suitability)
openairac-map-cli flightplan random --aircraft B744 --json

# Dual online network status (VATSIM & IVAO)
openairac-map-cli online all --json

# Airport weather query
openairac-map-cli weather KJFK --json
```
