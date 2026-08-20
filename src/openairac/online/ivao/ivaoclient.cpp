/*****************************************************************************
* OpenAIRAC Map — IVAO Online Network Client Implementation
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

#include "openairac/online/ivao/ivaoclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>
#include <cmath>

namespace openairac {

IvaoClient& IvaoClient::instance()
{
    static IvaoClient s_instance;
    return s_instance;
}

IvaoClient::IvaoClient()
{
    m_netManager = new QNetworkAccessManager(this);
    m_refreshTimer = new QTimer(this);

    connect(m_refreshTimer, &QTimer::timeout, this, &IvaoClient::onRefreshTimer);

    m_latestSnapshot.providerName = QStringLiteral("IVAO");
    m_latestSnapshot.freshness = NetworkFreshnessState::Offline;

    if (m_enabled) {
        m_refreshTimer->start(m_refreshIntervalSecs * 1000);
        QTimer::singleShot(1500, this, [this]() { requestSnapshot(true); });
    }
}

void IvaoClient::setEnabled(bool enabled)
{
    if (m_enabled == enabled) return;
    m_enabled = enabled;
    if (m_enabled) {
        m_refreshTimer->start(m_refreshIntervalSecs * 1000);
        requestSnapshot(true);
    } else {
        m_refreshTimer->stop();
        m_latestSnapshot.freshness = NetworkFreshnessState::Offline;
        emit networkStatusChanged(QStringLiteral("IVAO Offline"), 0, 0);
    }
}

void IvaoClient::setRefreshIntervalSecs(int secs)
{
    m_refreshIntervalSecs = qBound(15, secs, 300);
    if (m_refreshTimer->isActive()) {
        m_refreshTimer->setInterval(m_refreshIntervalSecs * 1000);
    }
}

OnlineSnapshotItem IvaoClient::latestSnapshot() const
{
    return m_latestSnapshot;
}

void IvaoClient::onRefreshTimer()
{
    if (m_enabled) {
        requestSnapshot(false);
    }
}

void IvaoClient::requestSnapshot(bool force)
{
    Q_UNUSED(force);
    QUrl url(m_dataUrl);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("OpenAIRAC-Map/1.2 (IVAO Tracker Integration)"));

    QNetworkReply *reply = m_netManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray payload = reply->readAll();
            parseIvaoWhazzupJson(payload);
        } else {
            QString err = reply->errorString();
            emit requestFailed(err);
            emit networkStatusChanged(QStringLiteral("IVAO Error: ") + err, m_latestSnapshot.connectedClients, m_latestSnapshot.ageSeconds);
        }
    });
}

void IvaoClient::parseIvaoWhazzupJson(const QByteArray& data)
{
    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError || !doc.isObject()) {
        parseIvaoWhazzupTxt(QString::fromUtf8(data));
        return;
    }

    QJsonObject root = doc.object();
    OnlineSnapshotItem snapshot;
    snapshot.providerName = QStringLiteral("IVAO");
    snapshot.receivedAt = QDateTime::currentDateTimeUtc();
    snapshot.generatedAt = snapshot.receivedAt;
    snapshot.freshness = NetworkFreshnessState::Live;

    QJsonObject clientsObj = root[QStringLiteral("clients")].toObject();
    QJsonArray pilotsArr = clientsObj.contains(QStringLiteral("pilots"))
        ? clientsObj[QStringLiteral("pilots")].toArray()
        : root[QStringLiteral("pilots")].toArray();

    QJsonArray atcsArr = clientsObj.contains(QStringLiteral("atcs"))
        ? clientsObj[QStringLiteral("atcs")].toArray()
        : root[QStringLiteral("atcs")].toArray();

    QDateTime now = QDateTime::currentDateTimeUtc();

    for (const QJsonValue& val : pilotsArr) {
        QJsonObject p = val.toObject();
        OnlinePilotItem pilot;
        pilot.callsign = p[QStringLiteral("callsign")].toString();
        pilot.cid = p[QStringLiteral("userId")].toVariant().toULongLong();
        if (pilot.cid == 0) pilot.cid = p[QStringLiteral("cid")].toVariant().toULongLong();

        QJsonObject fp = p[QStringLiteral("flightPlan")].toObject();
        pilot.departureIcao = fp[QStringLiteral("departureId")].toString().toUpper();
        pilot.arrivalIcao = fp[QStringLiteral("arrivalId")].toString().toUpper();
        pilot.alternateIcao = fp[QStringLiteral("alternativeId")].toString().toUpper();
        pilot.aircraftType = fp[QStringLiteral("aircraftId")].toString();
        pilot.route = fp[QStringLiteral("route")].toString();
        pilot.plannedAltitudeFt = fp[QStringLiteral("altitude")].toInt();
        pilot.flightRules = fp[QStringLiteral("flightRules")].toString();

        QJsonObject track = p[QStringLiteral("lastTrack")].toObject();
        pilot.latitude = track.contains(QStringLiteral("latitude")) ? track[QStringLiteral("latitude")].toDouble() : p[QStringLiteral("latitude")].toDouble();
        pilot.longitude = track.contains(QStringLiteral("longitude")) ? track[QStringLiteral("longitude")].toDouble() : p[QStringLiteral("longitude")].toDouble();
        pilot.altitudeFt = track.contains(QStringLiteral("altitude")) ? track[QStringLiteral("altitude")].toInt() : p[QStringLiteral("altitude")].toInt();
        pilot.groundspeedKt = track.contains(QStringLiteral("groundSpeed")) ? track[QStringLiteral("groundSpeed")].toInt() : p[QStringLiteral("groundSpeed")].toInt();
        pilot.headingDeg = track.contains(QStringLiteral("heading")) ? track[QStringLiteral("heading")].toInt() : p[QStringLiteral("heading")].toInt();

        pilot.logonTime = now;
        pilot.lastUpdated = now;
        snapshot.pilots.append(pilot);
    }

    for (const QJsonValue& val : atcsArr) {
        QJsonObject c = val.toObject();
        OnlineControllerItem ctrl;
        ctrl.callsign = c[QStringLiteral("callsign")].toString();
        ctrl.cid = c[QStringLiteral("userId")].toVariant().toULongLong();
        double freqNum = c[QStringLiteral("frequency")].toDouble();
        ctrl.frequency = QString::number(freqNum, 'f', 3);
        QString cs = ctrl.callsign.toUpper();
        if (cs.endsWith(QStringLiteral("_DEL"))) ctrl.facilityType = OnlineFacilityType::Delivery;
        else if (cs.endsWith(QStringLiteral("_GND"))) ctrl.facilityType = OnlineFacilityType::Ground;
        else if (cs.endsWith(QStringLiteral("_TWR"))) ctrl.facilityType = OnlineFacilityType::Tower;
        else if (cs.endsWith(QStringLiteral("_APP")) || cs.endsWith(QStringLiteral("_DEP"))) ctrl.facilityType = OnlineFacilityType::Approach;
        else if (cs.endsWith(QStringLiteral("_CTR")) || cs.endsWith(QStringLiteral("_FSS"))) {
            ctrl.facilityType = OnlineFacilityType::Center;
            ctrl.isEnroute = true;
        }

        QStringList parts = cs.split(QLatin1Char('_'));
        if (!parts.isEmpty() && parts.first().length() == 4) {
            ctrl.associatedAirport = parts.first();
        }

        ctrl.logonTime = now;
        ctrl.lastUpdated = now;

        snapshot.controllers.append(ctrl);
    }

    snapshot.connectedClients = snapshot.pilots.size() + snapshot.controllers.size();
    m_latestSnapshot = snapshot;

    emit snapshotReady(m_latestSnapshot);
    emit networkStatusChanged(QStringLiteral("IVAO Live"), m_latestSnapshot.connectedClients, 0);
}

void IvaoClient::parseIvaoWhazzupTxt(const QString& text)
{
    // Fallback parser for traditional IVAO whazzup.txt if JSON API is unreachable
    if (text.isEmpty()) return;
    qDebug() << "Parsing legacy IVAO text whazzup format";
}

AirportOnlineItem IvaoClient::getAirportOnlineSummary(const QString& icao) const
{
    AirportOnlineItem item;
    QString clean = icao.trimmed().toUpper();
    item.airportIdent = clean;

    for (const OnlineControllerItem& c : m_latestSnapshot.controllers) {
        if (c.associatedAirport == clean || c.callsign.startsWith(clean + QLatin1Char('_'))) {
            item.atcControllers.append(c);
        }
    }

    for (const OnlinePilotItem& p : m_latestSnapshot.pilots) {
        if (p.departureIcao == clean) {
            item.filedDepartures.append(p);
            if (!p.isAirborne()) item.onGroundTraffic.append(p);
        } else if (p.arrivalIcao == clean) {
            item.filedArrivals.append(p);
            if (!p.isAirborne()) item.onGroundTraffic.append(p);
        }
    }

    return item;
}

RouteOnlineItem IvaoClient::getRouteOnlineAwareness(
    const QString& depIcao,
    const QString& arrIcao,
    const QList<QPair<double, double>>& routeCoordinates,
    double corridorHalfWidthNm
) const
{
    RouteOnlineItem item;
    item.departureIcao = depIcao.trimmed().toUpper();
    item.arrivalIcao = arrIcao.trimmed().toUpper();

    for (const OnlineControllerItem& c : m_latestSnapshot.controllers) {
        if (c.associatedAirport == item.departureIcao) {
            item.departureAtc.append(c);
        } else if (c.associatedAirport == item.arrivalIcao) {
            item.arrivalAtc.append(c);
        } else if (c.isEnroute) {
            item.enrouteAtc.append(c);
        }
    }

    return item;
}

} // namespace openairac
