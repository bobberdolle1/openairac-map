/*****************************************************************************
* OpenAIRAC Map — Active Flight Workspace Dock Implementation
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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

namespace openairac {

ActiveFlightDock::ActiveFlightDock(QWidget *parent)
    : QDockWidget(tr("Active Flight [EFB]"), parent) {
    setObjectName(QStringLiteral("OpenAIRACActiveFlightDock"));

    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // 1. Flight Phase Box
    QGroupBox *phaseBox = new QGroupBox(tr("Flight Phase & Automation"), this);
    QVBoxLayout *phaseLayout = new QVBoxLayout(phaseBox);

    m_phaseBadge = new QLabel(QStringLiteral("PREFLIGHT"), this);
    m_phaseBadge->setStyleSheet(QStringLiteral("background: #007bff; color: white; font-weight: bold; font-size: 14px; padding: 4px 8px; border-radius: 4px;"));
    m_phaseBadge->setAlignment(Qt::AlignCenter);
    phaseLayout->addWidget(m_phaseBadge);

    m_evidenceLabel = new QLabel(tr("On ground, stationary"), this);
    m_evidenceLabel->setStyleSheet(QStringLiteral("color: #555; font-size: 11px;"));
    m_evidenceLabel->setWordWrap(true);
    phaseLayout->addWidget(m_evidenceLabel);
    mainLayout->addWidget(phaseBox);

    // 2. Route Progress Box
    QGroupBox *routeBox = new QGroupBox(tr("Route Navigation Progress"), this);
    QVBoxLayout *routeLayout = new QVBoxLayout(routeBox);

    m_routeLabel = new QLabel(tr("Route: Not active"), this);
    m_routeLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 12px;"));
    routeLayout->addWidget(m_routeLabel);

    m_progressLabel = new QLabel(tr("Next Waypoint: --- | Dist: --- | ETA: ---"), this);
    routeLayout->addWidget(m_progressLabel);

    QHBoxLayout *navStatsLayout = new QHBoxLayout();
    m_xtkLabel = new QLabel(tr("XTK: 0.0 NM ON"), this);
    m_todLabel = new QLabel(tr("Planning TOD: ---"), this);
    navStatsLayout->addWidget(m_xtkLabel);
    navStatsLayout->addWidget(m_todLabel);
    routeLayout->addLayout(navStatsLayout);
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
    QGroupBox *destBox = new QGroupBox(tr("Destination Intelligence"), this);
    QVBoxLayout *destLayout = new QVBoxLayout(destBox);

    m_destSummaryBrowser = new QTextBrowser(this);
    m_destSummaryBrowser->setMaximumHeight(110);
    destLayout->addWidget(m_destSummaryBrowser);
    mainLayout->addWidget(destBox);

    mainLayout->addStretch();
    setWidget(container);

    connect(&FlightPhaseEngine::instance(), &FlightPhaseEngine::phaseChanged, this, &ActiveFlightDock::onFlightPhaseChanged);
    connect(&ChartContextEngine::instance(), &ChartContextEngine::chartSuggested, this, &ActiveFlightDock::onChartSuggested);
}

void ActiveFlightDock::setFlightPlan(
    const QString& depIcao,
    const QString& destIcao,
    const QStringList& routeWaypoints,
    const QList<QPair<double, double>>& routeCoordinates,
    double destElevationFt
) {
    m_depIcao = depIcao.trimmed().toUpper();
    m_destIcao = destIcao.trimmed().toUpper();
    m_waypoints = routeWaypoints;
    m_routeCoords = routeCoordinates;
    m_destElevationFt = destElevationFt;

    m_routeLabel->setText(QStringLiteral("Route: ") + m_depIcao + QStringLiteral(" &rarr; ") + m_destIcao);
    ChartContextEngine::instance().setFlightPlanContext(m_depIcao, m_destIcao);

    updateDisplay();
}

void ActiveFlightDock::onFlightPhaseChanged(FlightPhase oldPhase, FlightPhase newPhase, const QString& evidence) {
    Q_UNUSED(oldPhase);
    m_phase = newPhase;
    m_evidence = evidence;

    m_phaseBadge->setText(flightPhaseToString(m_phase));
    m_evidenceLabel->setText(evidence);

    // Evaluate contextual chart for new phase
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

void ActiveFlightDock::updateDisplay() {
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
        html += QStringLiteral("<br/><b>") + m_destIcao + QStringLiteral(" VATSIM ATC:</b> ");
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
