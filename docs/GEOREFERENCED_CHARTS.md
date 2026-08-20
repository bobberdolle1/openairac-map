# OpenAIRAC Map — Georeferenced Raster Charts Specification

## 1. Supported Formats

OpenAIRAC Map supports calibrated raster charts (e.g. FAA GeoTIFF VFR Sectionals, Terminal Area Charts, IFR Enroute charts) through `openairac::GeoRasterLayer`.

## 2. Georeference Invariants

1. **Affine Inversion**: Geographic coordinates $(lon, lat)$ are mapped to raster pixels $(px, py)$ using the calibrated 6-parameter affine matrix.
2. **Ownship Overlay**: Simulator aircraft position is rendered directly on calibrated rasters when within bounds, accompanied by the `[GEOREFERENCED]` badge.
3. **Honest d-TPP Separation**: Terminal procedure PDFs (SIDs, STARs, IAPs) without official embedded GeoTIFF tags are displayed in the PDF Viewer without synthetic GPS position overlays.
