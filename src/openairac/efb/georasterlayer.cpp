/*****************************************************************************
* OpenAIRAC Map — Georeferenced Raster Map Layer Implementation
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

#include "openairac/efb/georasterlayer.h"
#include <QtMath>
#include <QDebug>

namespace openairac {

GeoRasterLayer& GeoRasterLayer::instance() {
    static GeoRasterLayer inst;
    return inst;
}

GeoRasterLayer::GeoRasterLayer()
    : QObject(nullptr) {
}

void GeoRasterLayer::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit layerRepaintRequested();
    }
}

void GeoRasterLayer::setOpacity(double opacity) {
    m_opacity = qBound(0.0, opacity, 1.0);
    emit layerRepaintRequested();
}

void GeoRasterLayer::loadRasterAsset(const QString& imagePath, const GeoRasterMetadata& meta) {
    m_image.load(imagePath);
    m_meta = meta;
    if (!m_image.isNull()) {
        m_meta.pixelWidth = m_image.width();
        m_meta.pixelHeight = m_image.height();
        m_meta.isGeoreferenced = true;
        emit rasterLoaded(m_meta);
        emit layerRepaintRequested();
    }
}

void GeoRasterLayer::clearRaster() {
    m_image = QImage();
    m_meta = GeoRasterMetadata();
    emit rasterCleared();
    emit layerRepaintRequested();
}

bool GeoRasterLayer::coversPosition(double lon, double lat) const {
    if (!m_meta.isGeoreferenced) return false;
    return lon >= m_meta.minLon && lon <= m_meta.maxLon && lat >= m_meta.minLat && lat <= m_meta.maxLat;
}

QPair<double, double> GeoRasterLayer::geoToPixel(double lon, double lat) const {
    if (!m_meta.isGeoreferenced) {
        return QPair<double, double>(0.0, 0.0);
    }

    double det = m_meta.a * m_meta.e - m_meta.b * m_meta.d;
    if (qAbs(det) < 1e-18) {
        return QPair<double, double>(0.0, 0.0);
    }

    double dx = lon - m_meta.c;
    double dy = lat - m_meta.f;

    double px = (m_meta.e * dx - m_meta.b * dy) / det;
    double py = (-m_meta.d * dx + m_meta.a * dy) / det;

    return QPair<double, double>(px, py);
}

QPair<double, double> GeoRasterLayer::pixelToGeo(double px, double py) const {
    if (!m_meta.isGeoreferenced) {
        return QPair<double, double>(0.0, 0.0);
    }

    double lon = m_meta.a * px + m_meta.b * py + m_meta.c;
    double lat = m_meta.d * px + m_meta.e * py + m_meta.f;

    return QPair<double, double>(lon, lat);
}

} // namespace openairac
