/*****************************************************************************
* OpenAIRAC Map — Georeferenced Raster Map Layer
*
* Copyright 2026 OpenAIRAC Contributors
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*****************************************************************************/

#ifndef OPENAIRAC_GEORASTERLAYER_H
#define OPENAIRAC_GEORASTERLAYER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QPair>

namespace openairac {

struct GeoRasterMetadata {
    QString id;
    QString productName;
    QString edition;
    double minLon = -180.0;
    double minLat = -90.0;
    double maxLon = 180.0;
    double maxLat = 90.0;
    int pixelWidth = 0;
    int pixelHeight = 0;
    double a = 0.0; // scale X
    double b = 0.0; // rotation X
    double c = 0.0; // origin X
    double d = 0.0; // rotation Y
    double e = 0.0; // scale Y
    double f = 0.0; // origin Y
    bool isGeoreferenced = false;
};

class GeoRasterLayer : public QObject {
    Q_OBJECT

public:
    static GeoRasterLayer& instance();

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    double opacity() const { return m_opacity; }
    void setOpacity(double opacity);

    void loadRasterAsset(const QString& imagePath, const GeoRasterMetadata& meta);
    void clearRaster();

    bool coversPosition(double lon, double lat) const;
    QPair<double, double> geoToPixel(double lon, double lat) const;
    QPair<double, double> pixelToGeo(double px, double py) const;

    const GeoRasterMetadata& metadata() const { return m_meta; }
    bool hasActiveRaster() const { return m_meta.isGeoreferenced && !m_image.isNull(); }

signals:
    void rasterLoaded(const openairac::GeoRasterMetadata& meta);
    void rasterCleared();
    void layerRepaintRequested();

private:
    GeoRasterLayer();
    virtual ~GeoRasterLayer() override = default;

    bool m_enabled = true;
    double m_opacity = 0.85;
    QImage m_image;
    GeoRasterMetadata m_meta;
};

} // namespace openairac

#endif // OPENAIRAC_GEORASTERLAYER_H
