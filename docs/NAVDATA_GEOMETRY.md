# OpenAIRAC Navdata Geometry Specification & Reality Gate

## 1. Principle of Authoritative Geodesic Centerline Geometry
OpenAIRAC establishes a strict mathematical contract for runway, ILS localizer, and procedure course calculations:

1. **Runway Heading is a Derived Geodesic Reality, Not a Designator Guess**:
   - The runway identifier (e.g. `02`, `03`, `07`, `18`, `28R`, `36`) is a nominal semantic identifier, **never raw degrees**.
   - True runway heading is calculated as the initial geodesic bearing from the Low-End threshold coordinates $(lat_{LE}, lon_{LE})$ to the High-End threshold coordinates $(lat_{HE}, lon_{HE})$:
     $$\theta = \text{atan2}(\sin \Delta \lambda \cdot \cos \phi_{HE}, \cos \phi_{LE} \cdot \sin \phi_{HE} - \sin \phi_{LE} \cdot \cos \phi_{HE} \cdot \cos \Delta \lambda)$$
   - Nominal heading from designator $(\text{runway\_number} \times 10)^\circ$ is strictly a secondary fallback used only when coordinates are missing or identical.

2. **Magnetic vs True Separation**:
   - Stored and exported runway headings are stored as true degrees $[0, 360)$.
   - Localizer courses are evaluated against true runway bearings.
   - WMM2025 declination is applied explicitly and never doubly added or subtracted.

3. **ILS Localizer and Glideslope Independence**:
   - Localizer geometry (azimuth feather) extends 10 NM along the reciprocal of the localizer true course:
     $$hdg_{opp} = (loc\_heading_{true} + 180^\circ) \pmod{360^\circ}$$
   - Glideslope angle (typically $3.00^\circ$) is maintained as distinct vertical descent data and never conflated with horizontal antenna coordinates.
