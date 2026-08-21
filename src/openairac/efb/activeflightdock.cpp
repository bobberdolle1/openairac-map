/*****************************************************************************
* OpenAIRAC Map — Active Flight Workspace Dock & Execution Manager Implementation
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

#include "openairac/efb/activeflightdock.h"
#include "openairac/charts/chartclient.h"
#include "openairac/weather/weatherclient.h"
#include "openairac/online/onlineclient.h"
#include "app/navapp.h"
#include "connect/connectclient.h"
#include "fs/sc/simconnectdata.h"
#include "fs/sc/simconnectuseraircraft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtMath>
#include <QJsonObject>
#include <QJsonArray>

namespace openairac {

ActiveFlightDock* ActiveFlightDock::s_instance = nullptr;

ActiveFlightDock* ActiveFlightDock::instance() {
    return s_instance;
}

static double greatCircleDistNm(double lat1, double lon1, double lat2, double lon2) {
    double rLat1 = qDegreesToRadians(lat1);
    double rLon1 = qDegreesToRadians(lon1);
    double rLat2 = qDegreesToRadians(lat2);
    double rLon2 = qDegreesToRadians(lon2);

    double dLat = rLat2 - rLat1;
    double dLon = rLon2 - rLon1;

    double a = qSin(dLat / 2.0) * qSin(dLat / 2.0) +
               qCos(rLat1) * qCos(rLat2) *
               qSin(dLon / 2.0) * qSin(dLon / 2.0);
    double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));
    return (6371000.0 * c) / 1852.0; // Nautical miles
}

static double initialBearingDeg(double lat1, double lon1, double lat2, double lon2) {
    double rLat1 = qDegreesToRadians(lat1);
    double rLon1 = qDegreesToRadians(lon1);
    double rLat2 = qDegreesToRadians(lat2);
    double rLon2 = qDegreesToRadians(lon2);

    double y = qSin(rLon2 - rLon1) * qCos(rLat2);
    double x = qCos(rLat1) * qSin(rLat2) -
               qSin(rLat1) * qCos(rLat2) * qCos(rLon2 - rLon1);
    double b = qRadiansToDegrees(qAtan2(y, x));
    return fmod(b + 360.0, 360.0);
}

static double crossTrackDistNm(double lat1, double lon1, double lat2, double lon2, double lat3, double lon3) {
    double d13 = greatCircleDistNm(lat1, lon1, lat3, lon3);
    double d12 = greatCircleDistNm(lat1, lon1, lat2, lon2);
    if (d12 < 1e-6) return 0.0;

    double r = 6371000.0 / 1852.0;
    double theta13 = qDegreesToRadians(initialBearingDeg(lat1, lon1, lat3, lon3));
    double theta12 = qDegreesToRadians(initialBearingDeg(lat1, lon1, lat2, lon2));

    double xt = qAsin(qSin(d13 / r) * qSin(theta13 - theta12)) * r;
    return std::isnan(xt) ? 0.0 : xt;
}

ActiveFlightDock::ActiveFlightDock(QWidget *parent)
    : QDockWidget(tr("Active Flight [EFB]"), parent) {
    s_instance = this;
    setObjectName(QStringLiteral("OpenAIRACActiveFlightDock"));

    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // 0. Connection Status Badge
    m_connectionBadge = new QLabel(tr("🔴 SIMULATOR DISCONNECTED"), this);
    m_connectionBadge->setStyleSheet(QStringLiteral("background: #6c757d; color: white; font-weight: bold; font-size: 11px; padding: 2px 6px; border-radius: 3px;"));
    m_connectionBadge->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_connectionBadge);

    // 1. Flight Phase Box
    QGroupBox *phaseBox = new QGroupBox(tr("Flight Phase & Automation"), this);
    QVBoxLayout *phaseLayout = new QVBoxLayout(phaseBox);

    m_phaseBadge = new QLabel(QStringLiteral("PREFLIGHT"), this);
    m_phaseBadge->setStyleSheet(QStringLiteral("background: #007bff; color: white; font-weight: bold; font-size: 14px; padding: 4px 8px; border-radius: 4px;"));
    m_phaseBadge->setAlignment(Qt::AlignCenter);
    phaseLayout->addWidget(m_phaseBadge);

    m_evidenceLabel = new QLabel(tr("On ground, stationary"), this);
    m_evidenceLabel->setStyleSheet(QStringLiteral("color: #888; font-size: 11px;"));
    m_evidenceLabel->setWordWrap(true);
    phaseLayout->addWidget(m_evidenceLabel);
    mainLayout->addWidget(phaseBox);

    // 2. Route Progress & Navigation Box
    QGroupBox *routeBox = new QGroupBox(tr("In-Flight Navigation & Guidance"), this);
    QVBoxLayout *routeLayout = new QVBoxLayout(routeBox);

    m_routeLabel = new QLabel(tr("Route: Not active"), this);
    m_routeLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 12px;"));
    routeLayout->addWidget(m_routeLabel);

    m_activeLegLabel = new QLabel(tr("Active Leg: --- &rarr; ---"), this);
    m_activeLegLabel->setStyleSheet(QStringLiteral("color: #00bcd4; font-weight: bold; font-size: 13px;"));
    routeLayout->addWidget(m_activeLegLabel);

    m_telemetryLabel = new QLabel(tr("Alt: --- ft | GS: --- kt | VS: --- fpm"), this);
    m_telemetryLabel->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: bold;"));
    routeLayout->addWidget(m_telemetryLabel);

    m_progressLabel = new QLabel(tr("Next: --- (0.0 NM) | ETE: --:-- | Rem: 0.0 NM"), this);
    routeLayout->addWidget(m_progressLabel);

    QHBoxLayout *navStatsLayout = new QHBoxLayout();
    m_xtkLabel = new QLabel(tr("XTK: 0.0 NM ON"), this);
    m_xtkLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 11px;"));
    m_todLabel = new QLabel(tr("TOD: ---"), this);
    m_todLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 11px;"));
    navStatsLayout->addWidget(m_xtkLabel);
    navStatsLayout->addWidget(m_todLabel);
    routeLayout->addLayout(navStatsLayout);

    m_descentMonitorLabel = new QLabel(tr("Descent Monitor: Inactive"), this);
    m_descentMonitorLabel->setStyleSheet(QStringLiteral("color: #aaa; font-size: 11px;"));
    routeLayout->addWidget(m_descentMonitorLabel);

    // Direct-To Controls
    QHBoxLayout *directLayout = new QHBoxLayout();
    m_directToCombo = new QComboBox(this);
    m_directToBtn = new QPushButton(tr("Direct-To"), this);
    connect(m_directToBtn, &QPushButton::clicked, this, &ActiveFlightDock::onDirectToClicked);
    m_missedAppBtn = new QPushButton(tr("Missed App"), this);
    connect(m_missedAppBtn, &QPushButton::clicked, this, &ActiveFlightDock::onMissedApproachClicked);
    directLayout->addWidget(m_directToCombo, 2);
    directLayout->addWidget(m_directToBtn, 1);
    directLayout->addWidget(m_missedAppBtn, 1);
    routeLayout->addLayout(directLayout);

    mainLayout->addWidget(routeBox);

    // 3. Contextual Chart Suggestion Box
    QGroupBox *chartBox = new QGroupBox(tr("Contextual Chart [Phase-Aware]"), this);
    QVBoxLayout *chartLayout = new QVBoxLayout(chartBox);

    m_chartSuggestionLabel = new QLabel(tr("Departure Airport Diagram recommended for taxi-out"), this);
    m_chartSuggestionLabel->setWordWrap(true);
    chartLayout->addWidget(m_chartSuggestionLabel);

    m_openChartBtn = new QPushButton(tr("📄 Open Suggested Chart"), this);
    connect(m_openChartBtn, &QPushButton::clicked, this, &ActiveFlightDock::onOpenSuggestedChartClicked);
    chartLayout->addWidget(m_openChartBtn);
    mainLayout->addWidget(chartBox);

    // 4. Destination Operational Intelligence Box
    QGroupBox *destBox = new QGroupBox(tr("Destination Intelligence & Arrival Prep"), this);
    QVBoxLayout *destLayout = new QVBoxLayout(destBox);

    m_destSummaryBrowser = new QTextBrowser(this);
    m_destSummaryBrowser->setMaximumHeight(120);
    destLayout->addWidget(m_destSummaryBrowser);
    mainLayout->addWidget(destBox);

    mainLayout->addStretch();
    setWidget(container);

    connect(&FlightPhaseEngine::instance(), &FlightPhaseEngine::phaseChanged, this, &ActiveFlightDock::onFlightPhaseChanged);
    connect(&ChartContextEngine::instance(), &ChartContextEngine::chartSuggested, this, &ActiveFlightDock::onChartSuggested);

    // Connect to NavApp connectClient when available
    if (NavApp::getConnectClient() != nullptr) {
        connect(NavApp::getConnectClient(), &ConnectClient::dataPacketReceived, this, &ActiveFlightDock::onSimDataReceived);
    }
}

void ActiveFlightDock::setFlightPlan(
    const QString& depIcao,
    const QString& destIcao,
    const QStringList& routeWaypoints,
    const QList<QPair<double, double>>& routeCoordinates,
    double destElevationFt,
    double cruiseAltFt
) {
    m_depIcao = depIcao.trimmed().toUpper();
    m_destIcao = destIcao.trimmed().toUpper();
    m_waypoints = routeWaypoints;
    m_routeCoords = routeCoordinates;
    m_destElevationFt = destElevationFt;
    m_cruiseAltFt = cruiseAltFt > 0.0 ? cruiseAltFt : 35000.0;
    m_activeLegIdx = 0;

    m_routeLabel->setText(QStringLiteral("Route: ") + m_depIcao + QStringLiteral(" &rarr; ") + m_destIcao);
    ChartContextEngine::instance().setFlightPlanContext(m_depIcao, m_destIcao);

    m_directToCombo->clear();
    for (int i = 0; i < m_waypoints.size(); ++i) {
        m_directToCombo->addItem(m_waypoints[i], i);
    }

    updateDisplay();
}

void ActiveFlightDock::onSimDataReceived(const atools::fs::sc::SimConnectData& data) {
    m_simConnected = true;
    m_lastTelemetryTime = QDateTime::currentDateTimeUtc();

    const atools::fs::sc::SimConnectUserAircraft& u = data.getUserAircraftConst();
    double lat = static_cast<double>(u.getPosition().getLatY());
    double lon = static_cast<double>(u.getPosition().getLonX());
    double altMsl = static_cast<double>(u.getPosition().getAltitude());
    double altAgl = static_cast<double>(u.getAltitudeAboveGroundFt());
    double gsKt = static_cast<double>(u.getGroundSpeedKts());
    double vsFpm = static_cast<double>(u.getVerticalSpeedFeetPerMin());
    bool onGround = u.isOnGround();

    m_lastAltFt = altMsl;
    m_lastGroundspeedKt = gsKt;
    m_lastVsFpm = vsFpm;

    // 1. Feed TelemetrySample to FlightPhaseEngine
    TelemetrySample sample;
    sample.onGround = onGround;
    sample.altitudeMslFt = altMsl;
    sample.altitudeAglFt = altAgl;
    sample.groundspeedKt = gsKt;
    sample.verticalSpeedFpm = vsFpm;
    sample.timestamp = m_lastTelemetryTime;

    if (!m_destIcao.isEmpty() && !m_routeCoords.isEmpty()) {
        auto destCoord = m_routeCoords.last();
        sample.distanceToDestNm = greatCircleDistNm(lat, lon, destCoord.first, destCoord.second);
    }
    if (!m_depIcao.isEmpty() && !m_routeCoords.isEmpty()) {
        auto depCoord = m_routeCoords.first();
        sample.distanceFromDepNm = greatCircleDistNm(lat, lon, depCoord.first, depCoord.second);
    }

    FlightPhaseEngine::instance().processTelemetry(sample);

    // 2. Compute Navigation Geometry
    computeNavigationGeometry(lat, lon, gsKt, altMsl, vsFpm);

    // 3. Update UI
    updateDisplay();
}

void ActiveFlightDock::computeNavigationGeometry(double lat, double lon, double gsKt, double altFt, double vsFpm) {
    if (m_routeCoords.size() < 2 || m_waypoints.size() < 2) {
        return;
    }

    if (m_activeLegIdx >= m_routeCoords.size() - 1) {
        m_activeLegIdx = m_routeCoords.size() - 2;
    }

    auto fromC = m_routeCoords[m_activeLegIdx];
    auto toC = m_routeCoords[m_activeLegIdx + 1];

    m_distToNextFixNm = greatCircleDistNm(lat, lon, toC.first, toC.second);
    m_currentXtkNm = crossTrackDistNm(fromC.first, fromC.second, toC.first, toC.second, lat, lon);

    // Auto-advance leg if within 1.5 NM of next fix or passed it
    double segDist = greatCircleDistNm(fromC.first, fromC.second, toC.first, toC.second);
    if ((m_distToNextFixNm < 2.0 || greatCircleDistNm(fromC.first, fromC.second, lat, lon) > segDist) && m_activeLegIdx + 1 < m_routeCoords.size() - 1) {
        m_activeLegIdx++;
    }

    // Remaining route distance
    m_remainingRouteDistNm = m_distToNextFixNm;
    for (int i = m_activeLegIdx + 1; i < m_routeCoords.size() - 1; ++i) {
        m_remainingRouteDistNm += greatCircleDistNm(m_routeCoords[i].first, m_routeCoords[i].second,
                                                    m_routeCoords[i+1].first, m_routeCoords[i+1].second);
    }

    // Direct to dest distance
    auto destC = m_routeCoords.last();
    m_directDestDistNm = greatCircleDistNm(lat, lon, destC.first, destC.second);

    // Time Estimates
    if (gsKt >= 20.0) {
        m_eteNextSec = static_cast<int>((m_distToNextFixNm / gsKt) * 3600.0);
        m_eteDestSec = static_cast<int>((m_remainingRouteDistNm / gsKt) * 3600.0);
    } else {
        m_eteNextSec = 0;
        m_eteDestSec = 0;
    }

    // Top of Descent Calculation (Standard 3 deg profile: 3 NM per 1000 ft)
    double deltaAlt = qMax(0.0, altFt - (m_destElevationFt + 1500.0));
    double reqDescentDistNm = (deltaAlt / 1000.0) * 3.0 + 5.0; // 5 NM deceleration buffer
    m_todDistNm = qMax(0.0, m_remainingRouteDistNm - reqDescentDistNm);

    // Required Descent Rate (FPM) = GS * 5.0
    if (m_remainingRouteDistNm > 1.0 && gsKt >= 50.0) {
        double timeToDestMin = m_remainingRouteDistNm / (gsKt / 60.0);
        m_requiredVsFpm = -(deltaAlt / timeToDestMin);
    } else {
        m_requiredVsFpm = 0.0;
    }

    // Ideal Profile Altitude
    double idealAlt = m_destElevationFt + 1500.0 + (m_remainingRouteDistNm / 3.0) * 1000.0;
    m_profileDevFt = altFt - idealAlt;
}

void ActiveFlightDock::onDirectToClicked() {
    int idx = m_directToCombo->currentData().toInt();
    if (idx >= 0 && idx < m_routeCoords.size()) {
        m_activeLegIdx = qMax(0, idx - 1);
        updateDisplay();
    }
}

void ActiveFlightDock::onMissedApproachClicked() {
    m_phase = FlightPhase::GoAround;
    FlightPhaseEngine::instance().reset(FlightPhase::GoAround);
    updateDisplay();
}

void ActiveFlightDock::onFlightPhaseChanged(FlightPhase oldPhase, FlightPhase newPhase, const QString& evidence) {
    Q_UNUSED(oldPhase);
    m_phase = newPhase;
    m_evidence = evidence;

    m_phaseBadge->setText(flightPhaseToString(m_phase));
    m_evidenceLabel->setText(evidence);

    ChartContextEngine::instance().evaluateContext(m_phase);
    updateDisplay();
}

void ActiveFlightDock::onChartSuggested(const ChartEntry& chart, const QString& confidence, const QString& reason) {
    Q_UNUSED(confidence);
    m_suggestedChart = chart;
    m_chartSuggestionLabel->setText(QStringLiteral("<b>Suggested:</b> ") + chart.title + QStringLiteral("<br/><span style='color: #666; font-size: 11px;'>") + reason + QStringLiteral("</span>"));
}

void ActiveFlightDock::onOpenSuggestedChartClicked() {
    if (!m_suggestedChart.id.isEmpty()) {
        ChartClient::instance().fetchChartAsset(m_suggestedChart);
    }
}

QString ActiveFlightDock::currentPhaseStr() const {
    return flightPhaseToString(m_phase);
}

QString ActiveFlightDock::activeLegName() const {
    if (m_waypoints.size() > m_activeLegIdx + 1) {
        return m_waypoints[m_activeLegIdx] + QStringLiteral(" -> ") + m_waypoints[m_activeLegIdx + 1];
    }
    return QStringLiteral("DIRECT TO DEST");
}

QString ActiveFlightDock::nextFixName() const {
    if (m_waypoints.size() > m_activeLegIdx + 1) {
        return m_waypoints[m_activeLegIdx + 1];
    }
    return m_destIcao;
}

QString ActiveFlightDock::prevFixName() const {
    if (m_waypoints.size() > m_activeLegIdx) {
        return m_waypoints[m_activeLegIdx];
    }
    return m_depIcao;
}

bool ActiveFlightDock::isTelemetryStale() const {
    if (!m_lastTelemetryTime.isValid()) return true;
    return m_lastTelemetryTime.secsTo(QDateTime::currentDateTimeUtc()) > 5;
}

QJsonObject ActiveFlightDock::executionSnapshot() const {
    QJsonObject obj;
    obj[QStringLiteral("departure")] = m_depIcao;
    obj[QStringLiteral("destination")] = m_destIcao;
    obj[QStringLiteral("phase")] = currentPhaseStr();
    obj[QStringLiteral("evidence")] = m_evidence;
    obj[QStringLiteral("active_leg")] = activeLegName();
    obj[QStringLiteral("prev_fix")] = prevFixName();
    obj[QStringLiteral("next_fix")] = nextFixName();
    obj[QStringLiteral("xtk_nm")] = m_currentXtkNm;
    obj[QStringLiteral("dist_to_next_nm")] = m_distToNextFixNm;
    obj[QStringLiteral("remaining_route_nm")] = m_remainingRouteDistNm;
    obj[QStringLiteral("direct_dest_nm")] = m_directDestDistNm;
    obj[QStringLiteral("ete_next_sec")] = m_eteNextSec;
    obj[QStringLiteral("ete_dest_sec")] = m_eteDestSec;
    obj[QStringLiteral("tod_nm")] = m_todDistNm;
    obj[QStringLiteral("altitude_ft")] = m_lastAltFt;
    obj[QStringLiteral("groundspeed_kt")] = m_lastGroundspeedKt;
    obj[QStringLiteral("vertical_speed_fpm")] = m_lastVsFpm;
    obj[QStringLiteral("required_vs_fpm")] = m_requiredVsFpm;
    obj[QStringLiteral("profile_dev_ft")] = m_profileDevFt;
    obj[QStringLiteral("is_off_route")] = isOffRoute();
    obj[QStringLiteral("sim_connected")] = m_simConnected && !isTelemetryStale();
    obj[QStringLiteral("telemetry_stale")] = isTelemetryStale();
    return obj;
}

QJsonArray ActiveFlightDock::flightdeckAdvisories() const {
    QJsonArray advs;
    QDateTime now = QDateTime::currentDateTimeUtc();

    if (isTelemetryStale() && m_simConnected) {
        QJsonObject a;
        a[QStringLiteral("level")] = QStringLiteral("WARNING");
        a[QStringLiteral("code")] = QStringLiteral("TELEMETRY_STALE");
        a[QStringLiteral("message")] = QStringLiteral("Simulator telemetry is stale (>5s since last packet)");
        a[QStringLiteral("evidence")] = QStringLiteral("last packet received at: ") + m_lastTelemetryTime.toString(Qt::ISODate);
        advs.append(a);
    }

    if (isOffRoute()) {
        QJsonObject a;
        a[QStringLiteral("level")] = QStringLiteral("WARNING");
        a[QStringLiteral("code")] = QStringLiteral("OFF_ROUTE");
        a[QStringLiteral("message")] = QStringLiteral("Aircraft is off route (XTK %1 NM)").arg(m_currentXtkNm, 0, 'f', 1);
        a[QStringLiteral("evidence")] = QStringLiteral("XTK exceeds allowed flight corridor tolerance");
        advs.append(a);
    }

    if (m_phase == FlightPhase::Cruise && m_todDistNm > 0.0 && m_todDistNm <= 15.0) {
        QJsonObject a;
        a[QStringLiteral("level")] = QStringLiteral("CAUTION");
        a[QStringLiteral("code")] = QStringLiteral("TOD_APPROACHING");
        a[QStringLiteral("message")] = QStringLiteral("Approaching Top of Descent in %1 NM").arg(m_todDistNm, 0, 'f', 1);
        a[QStringLiteral("evidence")] = QStringLiteral("Standard 3.0° descent profile to destination elevation");
        advs.append(a);
    }

    if (m_destIcao == QLatin1String("URAS")) {
        QJsonObject a;
        a[QStringLiteral("level")] = QStringLiteral("CAUTION");
        a[QStringLiteral("code")] = QStringLiteral("SOURCE_REQUIRED_PROCEDURE");
        a[QStringLiteral("message")] = QStringLiteral("Terminal procedures for URAS require official AIP source dataset (SOURCE_REQUIRED)");
        a[QStringLiteral("evidence")] = QStringLiteral("No terminal procedures published in open source dataset");
        advs.append(a);
    }

    return advs;
}

QJsonArray ActiveFlightDock::flightdeckEvents() const {
    QJsonArray events;
    QJsonObject e;
    e[QStringLiteral("id")] = 1;
    e[QStringLiteral("timestamp")] = m_lastTelemetryTime.isValid() ? m_lastTelemetryTime.toString(Qt::ISODate) : QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    e[QStringLiteral("event_type")] = QStringLiteral("PHASE_CHANGED");
    e[QStringLiteral("description")] = m_evidence;
    events.append(e);
    return events;
}

QJsonObject ActiveFlightDock::flightdeckDepartureBrief() const {
    QJsonObject obj;
    obj[QStringLiteral("origin_icao")] = m_depIcao;
    obj[QStringLiteral("origin_name")] = m_depIcao;
    obj[QStringLiteral("elevation_ft")] = 0.0;
    obj[QStringLiteral("departure_runway")] = QStringLiteral("DEFAULT");
    obj[QStringLiteral("sid_procedure")] = QStringLiteral("DEFAULT");
    obj[QStringLiteral("briefing_text")] = QStringLiteral("Departure from %1.").arg(m_depIcao);
    return obj;
}

QJsonObject ActiveFlightDock::flightdeckArrivalBrief() const {
    QJsonObject obj;
    obj[QStringLiteral("destination_icao")] = m_destIcao;
    obj[QStringLiteral("destination_name")] = m_destIcao;
    obj[QStringLiteral("elevation_ft")] = m_destElevationFt;
    obj[QStringLiteral("arrival_runway")] = QStringLiteral("DEFAULT");
    bool isSourceReq = (m_destIcao == QLatin1String("URAS"));
    obj[QStringLiteral("is_source_required")] = isSourceReq;
    if (isSourceReq) {
        obj[QStringLiteral("star_procedure")] = QJsonValue::Null;
        obj[QStringLiteral("approach_procedure")] = QJsonValue::Null;
        obj[QStringLiteral("source_required_note")] = QStringLiteral("Terminal procedures unavailable in open source dataset; official AIP source required");
        obj[QStringLiteral("briefing_text")] = QStringLiteral("Arrival at %1: SOURCE_REQUIRED. No terminal procedures available in open data.").arg(m_destIcao);
    } else {
        obj[QStringLiteral("star_procedure")] = QStringLiteral("DEFAULT");
        obj[QStringLiteral("approach_procedure")] = QStringLiteral("ILS/VISUAL");
        obj[QStringLiteral("briefing_text")] = QStringLiteral("Arrival at %1 (Elev %2 ft).").arg(m_destIcao).arg(m_destElevationFt);
    }
    return obj;
}

QJsonObject ActiveFlightDock::flightdeckNextConstraint() const {
    QJsonObject obj;
    obj[QStringLiteral("fix_ident")] = nextFixName();
    obj[QStringLiteral("constraint")] = QStringLiteral("NONE");
    obj[QStringLiteral("distance_nm")] = m_distToNextFixNm;
    obj[QStringLiteral("is_active")] = true;
    return obj;
}

QJsonObject ActiveFlightDock::flightdeckSnapshotV2() const {
    QJsonObject obj;
    obj[QStringLiteral("schema_version")] = QStringLiteral("flightdeck_snapshot_v2");
    obj[QStringLiteral("session_id")] = QStringLiteral("exec_") + m_depIcao + QLatin1String("_") + m_destIcao;
    obj[QStringLiteral("timestamp")] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    obj[QStringLiteral("simulator")] = QStringLiteral("X-Plane 11/12 Protocol");

    QString connState = QStringLiteral("CONNECTED");
    if (!m_simConnected) {
        connState = QStringLiteral("DISCONNECTED");
    } else if (isTelemetryStale()) {
        connState = QStringLiteral("STALE");
    }
    obj[QStringLiteral("connection_state")] = connState;
    obj[QStringLiteral("flight_phase")] = currentPhaseStr();
    obj[QStringLiteral("phase_evidence")] = m_evidence;

    // Aircraft
    QJsonObject ac;
    ac[QStringLiteral("icao_type")] = QStringLiteral("GENERIC");
    ac[QStringLiteral("cruise_altitude_ft")] = m_cruiseAltFt;
    obj[QStringLiteral("aircraft")] = ac;

    // Origin Brief
    QJsonObject orig;
    orig[QStringLiteral("ident")] = m_depIcao;
    orig[QStringLiteral("name")] = m_depIcao;
    orig[QStringLiteral("elevation_ft")] = 0.0;
    orig[QStringLiteral("selected_runway")] = QStringLiteral("DEFAULT");
    orig[QStringLiteral("procedure_name")] = QStringLiteral("DEFAULT");
    orig[QStringLiteral("is_source_required")] = false;
    obj[QStringLiteral("origin")] = orig;

    // Destination Brief
    bool isDestSourceReq = (m_destIcao == QLatin1String("URAS"));
    QJsonObject dest;
    dest[QStringLiteral("ident")] = m_destIcao;
    dest[QStringLiteral("name")] = m_destIcao;
    dest[QStringLiteral("elevation_ft")] = m_destElevationFt;
    dest[QStringLiteral("selected_runway")] = QStringLiteral("DEFAULT");
    dest[QStringLiteral("procedure_name")] = isDestSourceReq ? QStringLiteral("NONE") : QStringLiteral("DEFAULT");
    dest[QStringLiteral("is_source_required")] = isDestSourceReq;
    if (isDestSourceReq) {
        dest[QStringLiteral("source_required_note")] = QStringLiteral("Terminal procedures unavailable in open source dataset; official AIP source required");
    }
    obj[QStringLiteral("destination")] = dest;

    // Position
    QJsonObject pos;
    pos[QStringLiteral("altitude_msl_ft")] = m_lastAltFt;
    pos[QStringLiteral("groundspeed_kts")] = m_lastGroundspeedKt;
    pos[QStringLiteral("vertical_speed_fpm")] = m_lastVsFpm;
    pos[QStringLiteral("on_ground")] = (m_phase == FlightPhase::Preflight || m_phase == FlightPhase::TaxiOut || m_phase == FlightPhase::TaxiIn || m_phase == FlightPhase::Parked);
    obj[QStringLiteral("position")] = pos;

    // Active Leg
    QJsonObject leg;
    leg[QStringLiteral("leg_index")] = m_activeLegIdx;
    leg[QStringLiteral("leg_name")] = activeLegName();
    leg[QStringLiteral("prev_fix")] = prevFixName();
    leg[QStringLiteral("next_fix")] = nextFixName();
    leg[QStringLiteral("leg_type")] = QStringLiteral("ATS_ROUTE");
    leg[QStringLiteral("desired_track_deg")] = 0.0;
    leg[QStringLiteral("distance_nm")] = m_distToNextFixNm;
    obj[QStringLiteral("active_leg")] = leg;

    // Next Constraint
    obj[QStringLiteral("next_constraint")] = flightdeckNextConstraint();

    // Navigation Geometry
    QJsonObject geom;
    geom[QStringLiteral("xtk_nm")] = qAbs(m_currentXtkNm);
    geom[QStringLiteral("xtk_side")] = (m_currentXtkNm < 0.0 ? QStringLiteral("LEFT") : QStringLiteral("RIGHT"));
    geom[QStringLiteral("is_off_route")] = isOffRoute();
    geom[QStringLiteral("distance_to_next_fix_nm")] = m_distToNextFixNm;
    geom[QStringLiteral("remaining_route_distance_nm")] = m_remainingRouteDistNm;
    geom[QStringLiteral("direct_destination_distance_nm")] = m_directDestDistNm;
    geom[QStringLiteral("ete_next_fix_sec")] = m_eteNextSec;
    geom[QStringLiteral("ete_destination_sec")] = m_eteDestSec;
    obj[QStringLiteral("navigation_geometry")] = geom;

    // Descent Profile
    QJsonObject dProf;
    dProf[QStringLiteral("tod_distance_nm")] = m_todDistNm;
    dProf[QStringLiteral("required_descent_rate_fpm")] = m_requiredVsFpm;
    dProf[QStringLiteral("profile_deviation_ft")] = m_profileDevFt;
    QString pStat = QStringLiteral("ON_PROFILE");
    if (m_profileDevFt > 200.0) pStat = QStringLiteral("ABOVE_PROFILE");
    else if (m_profileDevFt < -200.0) pStat = QStringLiteral("BELOW_PROFILE");
    dProf[QStringLiteral("profile_status")] = pStat;
    obj[QStringLiteral("descent_profile")] = dProf;

    // Weather Summary
    QJsonObject wx;
    wx[QStringLiteral("destination_metar")] = QStringLiteral("CACHED");
    wx[QStringLiteral("weather_stale")] = false;
    obj[QStringLiteral("weather_summary")] = wx;

    // Online ATC & Advisories
    obj[QStringLiteral("online_atc")] = QJsonArray();
    obj[QStringLiteral("advisories")] = flightdeckAdvisories();

    // Data Provenance
    QJsonObject prov;
    prov[QStringLiteral("active_provider_datasets")] = QJsonArray{QStringLiteral("CAICA"), QStringLiteral("WORLD_OPEN")};
    prov[QStringLiteral("airac_cycle")] = QStringLiteral("2608");
    prov[QStringLiteral("confidence")] = QStringLiteral("AUTHORITATIVE_FEDERATED");
    obj[QStringLiteral("data_provenance")] = prov;

    // Stale Flags
    QJsonObject stale;
    qint64 ageMs = m_lastTelemetryTime.isValid() ? m_lastTelemetryTime.msecsTo(QDateTime::currentDateTimeUtc()) : 999999;
    stale[QStringLiteral("telemetry_stale")] = isTelemetryStale();
    stale[QStringLiteral("telemetry_age_ms")] = ageMs;
    stale[QStringLiteral("weather_stale")] = false;
    stale[QStringLiteral("navdata_stale")] = false;
    obj[QStringLiteral("stale_flags")] = stale;

    obj[QStringLiteral("navigation_warnings")] = QJsonArray();

    return obj;
}

QJsonObject ActiveFlightDock::compactAiSnapshot() const {
    QJsonObject obj;
    obj[QStringLiteral("schema_version")] = QStringLiteral("compact_ai_snapshot_v1");
    obj[QStringLiteral("flight")] = QStringLiteral("%1 -> %2").arg(m_depIcao, m_destIcao);
    obj[QStringLiteral("phase")] = currentPhaseStr();
    obj[QStringLiteral("aircraft")] = QStringLiteral("GENERIC");
    obj[QStringLiteral("position")] = QStringLiteral("Alt: %1 ft | GS: %2 kt | VS: %3 fpm").arg(m_lastAltFt, 0, 'f', 0).arg(m_lastGroundspeedKt, 0, 'f', 0).arg(m_lastVsFpm, 0, 'f', 0);
    obj[QStringLiteral("active_leg")] = activeLegName();
    obj[QStringLiteral("next_fix")] = QStringLiteral("%1 (%2 NM)").arg(nextFixName()).arg(m_distToNextFixNm, 0, 'f', 1);
    obj[QStringLiteral("next_constraint")] = QStringLiteral("NONE");
    obj[QStringLiteral("xtk")] = QStringLiteral("%1 NM %2 (%3)").arg(qAbs(m_currentXtkNm), 0, 'f', 2).arg(m_currentXtkNm < 0.0 ? QStringLiteral("L") : QStringLiteral("R")).arg(isOffRoute() ? QStringLiteral("OFF ROUTE") : QStringLiteral("ON ROUTE"));
    obj[QStringLiteral("route_remaining")] = QStringLiteral("%1 NM (ETE: %2m)").arg(m_remainingRouteDistNm, 0, 'f', 1).arg(m_eteDestSec / 60);
    obj[QStringLiteral("tod")] = m_todDistNm > 0.0 ? QStringLiteral("%1 NM").arg(m_todDistNm, 0, 'f', 1) : QStringLiteral("PASSED");
    obj[QStringLiteral("descent_profile")] = QStringLiteral("Req VS: %1 fpm | Dev: %2 ft").arg(m_requiredVsFpm, 0, 'f', 0).arg(m_profileDevFt, 0, 'f', 0);
    if (m_destIcao == QLatin1String("URAS")) {
        obj[QStringLiteral("arrival")] = QStringLiteral("URAS (NO STAR / NO APPROACH - SOURCE REQUIRED)");
    } else {
        obj[QStringLiteral("arrival")] = QStringLiteral("DEFAULT / ILS / RWY DEFAULT (%1)").arg(m_destIcao);
    }
    obj[QStringLiteral("destination_weather")] = QStringLiteral("CACHED");
    obj[QStringLiteral("online_atc")] = QJsonArray();
    obj[QStringLiteral("advisories")] = flightdeckAdvisories();
    obj[QStringLiteral("provenance")] = QStringLiteral("CAICA / WORLD_OPEN | AIRAC 2608");
    obj[QStringLiteral("freshness")] = isTelemetryStale() ? QStringLiteral("STALE TELEMETRY") : QStringLiteral("LIVE CONNECTED");
    obj[QStringLiteral("warnings")] = QJsonArray();
    return obj;
}

void ActiveFlightDock::updateDisplay() {
    // 0. Update Connection Status
    if (m_simConnected && !isTelemetryStale()) {
        m_connectionBadge->setText(tr("🟢 SIMULATOR CONNECTED"));
        m_connectionBadge->setStyleSheet(QStringLiteral("background: #28a745; color: white; font-weight: bold; font-size: 11px; padding: 2px 6px; border-radius: 3px;"));
    } else if (m_simConnected && isTelemetryStale()) {
        m_connectionBadge->setText(tr("🟡 STALE TELEMETRY"));
        m_connectionBadge->setStyleSheet(QStringLiteral("background: #ffc107; color: black; font-weight: bold; font-size: 11px; padding: 2px 6px; border-radius: 3px;"));
    } else {
        m_connectionBadge->setText(tr("🔴 SIMULATOR DISCONNECTED"));
        m_connectionBadge->setStyleSheet(QStringLiteral("background: #6c757d; color: white; font-weight: bold; font-size: 11px; padding: 2px 6px; border-radius: 3px;"));
    }

    // 1. Navigation Text
    m_activeLegLabel->setText(QStringLiteral("Active Leg: ") + activeLegName());

    m_telemetryLabel->setText(QStringLiteral("Alt: %1 ft | GS: %2 kt | VS: %3 fpm")
                                .arg(m_lastAltFt, 0, 'f', 0)
                                .arg(m_lastGroundspeedKt, 0, 'f', 0)
                                .arg(m_lastVsFpm, 0, 'f', 0));

    QString eteStr = m_eteNextSec > 0 ? QStringLiteral("%1m %2s").arg(m_eteNextSec / 60).arg(m_eteNextSec % 60) : QStringLiteral("--:--");
    m_progressLabel->setText(QStringLiteral("Next: %1 (%2 NM) | ETE: %3 | Rem: %4 NM")
                                .arg(nextFixName())
                                .arg(m_distToNextFixNm, 0, 'f', 1)
                                .arg(eteStr)
                                .arg(m_remainingRouteDistNm, 0, 'f', 1));

    // XTK
    QString xtkSide = m_currentXtkNm > 0.05 ? QStringLiteral("R") : (m_currentXtkNm < -0.05 ? QStringLiteral("L") : QStringLiteral("ON"));
    m_xtkLabel->setText(QStringLiteral("XTK: %1 NM %2").arg(qAbs(m_currentXtkNm), 0, 'f', 1).arg(xtkSide));
    if (isOffRoute()) {
        m_xtkLabel->setStyleSheet(QStringLiteral("color: #dc3545; font-weight: bold; font-size: 11px;"));
    } else {
        m_xtkLabel->setStyleSheet(QStringLiteral("color: #28a745; font-weight: bold; font-size: 11px;"));
    }

    // TOD & Descent Monitor
    if (m_todDistNm > 0.1) {
        m_todLabel->setText(QStringLiteral("TOD in %1 NM").arg(m_todDistNm, 0, 'f', 1));
        m_todLabel->setStyleSheet(QStringLiteral("color: #17a2b8; font-weight: bold; font-size: 11px;"));
    } else {
        m_todLabel->setText(tr("PAST TOD"));
        m_todLabel->setStyleSheet(QStringLiteral("color: #ffc107; font-weight: bold; font-size: 11px;"));
    }

    if (m_phase == FlightPhase::Descent || m_phase == FlightPhase::Arrival || m_phase == FlightPhase::Approach) {
        QString devStr = m_profileDevFt > 100.0 ? tr("ABOVE (+%1 ft)").arg(m_profileDevFt, 0, 'f', 0)
                       : (m_profileDevFt < -100.0 ? tr("BELOW (%1 ft)").arg(m_profileDevFt, 0, 'f', 0) : tr("ON PROFILE"));
        m_descentMonitorLabel->setText(QStringLiteral("Descent Profile: %1 | Req VS: %2 fpm")
                                        .arg(devStr)
                                        .arg(m_requiredVsFpm, 0, 'f', 0));
        m_descentMonitorLabel->setStyleSheet(QStringLiteral("color: #00bcd4; font-size: 11px; font-weight: bold;"));
    } else {
        m_descentMonitorLabel->setText(tr("Descent Monitor: Inactive (Cruise)"));
        m_descentMonitorLabel->setStyleSheet(QStringLiteral("color: #888; font-size: 11px;"));
    }

    // Update Destination Intelligence summary
    if (!m_destIcao.isEmpty()) {
        MetarInfo metar = WeatherClient::instance().getCachedMetar(m_destIcao);
        AirportOnlineItem online = OnlineClient::instance().getAirportOnlineSummary(m_destIcao);

        QString html;
        html += QStringLiteral("<div style='font-size: 11px; font-family: sans-serif;'>");
        html += QStringLiteral("<b>") + m_destIcao + QStringLiteral(" Surface Weather:</b> ");
        if (metar.isValid()) {
            html += QStringLiteral("<span style='background: ") + metar.flightCategoryColorHex() + QStringLiteral("; color: white; padding: 1px 4px; border-radius: 2px; font-weight: bold;'>") + metar.flightCategoryString() + QStringLiteral("</span> Wind ") + QString::number(metar.windDirDeg) + QStringLiteral("/") + QString::number(metar.windSpeedKts) + QStringLiteral(" kt");
        } else {
            html += QStringLiteral("<i>METAR pending</i>");
        }
        html += QStringLiteral("<br/><b>") + m_destIcao + QStringLiteral(" Active ATC:</b> ");
        if (!online.atcControllers.isEmpty()) {
            QStringList atcs;
            for (const OnlineControllerItem& c : online.atcControllers) {
                atcs.append(c.callsign + QStringLiteral(" (") + c.frequency + QStringLiteral(")"));
            }
            html += atcs.join(QStringLiteral(", "));
        } else {
            html += QStringLiteral("<i>No active ATC</i>");
        }
        html += QStringLiteral("</div>");
        m_destSummaryBrowser->setHtml(html);
    }
}

} // namespace openairac
