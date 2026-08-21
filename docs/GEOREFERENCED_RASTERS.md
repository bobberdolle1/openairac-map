# OpenAIRAC Georeferenced Raster Specification (v1.9.0)

## 1. Overview & Capabilities

OpenAIRAC v1.9.0 introduces official support for **Geospatial Raster Charts** (`openairac-charts::georaster`), enabling high-resolution visual navigation charts (e.g. FAA VFR Sectionals, Terminal Area Charts, IFR Enroute GeoTIFFs) to be calibrated, validated, and overlaid with live simulator ownship telemetry.

### Core Guarantees:
1. **6-Parameter Affine Transform**: Rigorous mathematical transformation matrix ($a, b, c, d, e, f$) mapping pixel coordinates to geographic WGS84 coordinates.
2. **Invertible Precision Validation**: Every calibrated raster must pass automated round-trip validation ($(\text{px}, \text{py}) \to (\text{lon}, \text{lat}) \to (\text{px}, \text{py})$) with residual error $\le 0.001\text{ pixels}$.
3. **No Guessing / No Faking**: Standard FAA terminal d-TPP PDFs (SIDs, STARs, IAPs, Airport Diagrams) remain classified as `NotGeoreferenced` unless official authoritative coordinate transforms are supplied by the aviation authority. OpenAIRAC **never guesses georeference transforms via OCR or corner-fitting**.

---

## 2. Mathematical Coordinate Transform

The 2D affine transformation is defined as:

$$\begin{aligned}
\text{Lon} &= a \cdot x_{\text{pixel}} + b \cdot y_{\text{pixel}} + c \\
\text{Lat} &= d \cdot x_{\text{pixel}} + e \cdot y_{\text{pixel}} + f
\end{aligned}$$

Where:
* $a$: Horizontal pixel scale ($\text{degrees}/\text{pixel}$)
* $e$: Vertical pixel scale (negative for north-up rasters)
* $b, d$: Rotation / shearing parameters
* $c, f$: Coordinate origin corresponding to pixel $(0, 0)$

The determinant $\det = a \cdot e - b \cdot d$ is strictly validated to be non-zero and non-singular before asset registration.

---

## 3. Data Structures (`openairac_charts::georaster`)

```rust
pub struct GeoRasterAsset {
    pub id: String,
    pub provider: String,
    pub product_name: String,
    pub edition: String,
    pub effective_from: DateTime<Utc>,
    pub effective_to: Option<DateTime<Utc>>,
    pub crs_epsg: u32,
    pub bounds: GeoBounds,
    pub pixel_width: u32,
    pub pixel_height: u32,
    pub affine_transform: AffineTransform,
    pub sha256_hash: String,
    pub status: GeoreferenceStatus,
    pub source_url: Option<String>,
}
```

---

## 4. Supported FAA Products

* **FAA VFR Sectional Charts** (GeoTIFF)
* **FAA VFR Terminal Area Charts (TAC)** (GeoTIFF)
* **FAA IFR Enroute Low / High Altitude Charts** (GeoTIFF)
