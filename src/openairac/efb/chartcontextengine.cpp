/*****************************************************************************
* OpenAIRAC Map — Phase-Aware Chart Context Engine Implementation
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

#include "openairac/efb/chartcontextengine.h"
#include "openairac/charts/chartclient.h"

namespace openairac {

ChartContextEngine& ChartContextEngine::instance() {
    static ChartContextEngine inst;
    return inst;
}

ChartContextEngine::ChartContextEngine()
    : QObject(nullptr) {
}

void ChartContextEngine::setFlightPlanContext(
    const QString& depIcao,
    const QString& destIcao,
    const QString& sidName,
    const QString& starName,
    const QString& appName,
    const QString& depRwy,
    const QString& destRwy
) {
    m_depIcao = depIcao.trimmed().toUpper();
    m_destIcao = destIcao.trimmed().toUpper();
    m_sidName = sidName.trimmed().toUpper();
    m_starName = starName.trimmed().toUpper();
    m_appName = appName.trimmed().toUpper();
    m_depRwy = depRwy.trimmed().toUpper();
    m_destRwy = destRwy.trimmed().toUpper();
}

void ChartContextEngine::pinChart(const ChartEntry& chart) {
    if (!m_pinnedIds.contains(chart.id)) {
        m_pinnedCharts.append(chart);
        m_pinnedIds.insert(chart.id);
    }
}

void ChartContextEngine::unpinChart(const QString& chartId) {
    m_pinnedIds.remove(chartId);
    for (int i = 0; i < m_pinnedCharts.size(); ++i) {
        if (m_pinnedCharts[i].id == chartId) {
            m_pinnedCharts.removeAt(i);
            break;
        }
    }
}

bool ChartContextEngine::isPinned(const QString& chartId) const {
    return m_pinnedIds.contains(chartId);
}

ContextualChartResult ChartContextEngine::evaluateContext(FlightPhase phase) {
    ContextualChartResult res;

    switch (phase) {
    case FlightPhase::Preflight:
    case FlightPhase::TaxiOut: {
        if (m_depIcao.isEmpty()) break;
        QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_depIcao);
        for (const ChartEntry& c : charts) {
            if (c.type == ChartType::AirportDiagram) {
                res.chart = c;
                res.confidence = AssociationConfidence::Exact;
                res.reason = QStringLiteral("Departure Airport Diagram for taxi-out");
                break;
            }
        }
        break;
    }
    case FlightPhase::Takeoff:
    case FlightPhase::InitialClimb:
    case FlightPhase::Departure: {
        if (m_depIcao.isEmpty()) break;
        QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_depIcao);
        QList<ChartEntry> sidMatches;
        for (const ChartEntry& c : charts) {
            if (c.type == ChartType::Sid) {
                if (!m_sidName.isEmpty() && (c.procedureIdent == m_sidName || c.title.toUpper().contains(m_sidName))) {
                    res.chart = c;
                    res.confidence = AssociationConfidence::Exact;
                    res.reason = QStringLiteral("Active Departure Procedure (SID): %1").arg(m_sidName);
                    sidMatches.clear();
                    break;
                }
                sidMatches.append(c);
            }
        }
        if (res.confidence == AssociationConfidence::Unresolved && !sidMatches.isEmpty()) {
            if (sidMatches.size() == 1) {
                res.chart = sidMatches.first();
                res.confidence = AssociationConfidence::Likely;
                res.reason = QStringLiteral("Departure Procedure chart");
            } else {
                res.isAmbiguous = true;
                res.ambiguousCandidates = sidMatches;
                res.reason = QStringLiteral("Multiple departure charts available");
            }
        }
        break;
    }
    case FlightPhase::Descent:
    case FlightPhase::Arrival: {
        if (m_destIcao.isEmpty()) break;
        QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_destIcao);
        QList<ChartEntry> starMatches;
        for (const ChartEntry& c : charts) {
            if (c.type == ChartType::Star) {
                if (!m_starName.isEmpty() && (c.procedureIdent == m_starName || c.title.toUpper().contains(m_starName))) {
                    res.chart = c;
                    res.confidence = AssociationConfidence::Exact;
                    res.reason = QStringLiteral("Active Arrival Procedure (STAR): %1").arg(m_starName);
                    starMatches.clear();
                    break;
                }
                starMatches.append(c);
            }
        }
        if (res.confidence == AssociationConfidence::Unresolved && !starMatches.isEmpty()) {
            if (starMatches.size() == 1) {
                res.chart = starMatches.first();
                res.confidence = AssociationConfidence::Likely;
                res.reason = QStringLiteral("Terminal Arrival chart");
            } else {
                res.isAmbiguous = true;
                res.ambiguousCandidates = starMatches;
                res.reason = QStringLiteral("Multiple arrival charts available");
            }
        }
        break;
    }
    case FlightPhase::Approach:
    case FlightPhase::Final: {
        if (m_destIcao.isEmpty()) break;
        QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_destIcao);
        QList<ChartEntry> appMatches;
        for (const ChartEntry& c : charts) {
            if (c.type == ChartType::Approach) {
                if (!m_appName.isEmpty() && (c.procedureIdent == m_appName || c.title.toUpper().contains(m_appName))) {
                    res.chart = c;
                    res.confidence = AssociationConfidence::Exact;
                    res.reason = QStringLiteral("Active Instrument Approach: %1").arg(m_appName);
                    appMatches.clear();
                    break;
                }
                if (!m_destRwy.isEmpty() && c.runway == m_destRwy) {
                    appMatches.append(c);
                }
            }
        }
        if (res.confidence == AssociationConfidence::Unresolved && !appMatches.isEmpty()) {
            if (appMatches.size() == 1) {
                res.chart = appMatches.first();
                res.confidence = AssociationConfidence::Likely;
                res.reason = QStringLiteral("Approach chart matching runway %1").arg(m_destRwy);
            } else {
                res.isAmbiguous = true;
                res.ambiguousCandidates = appMatches;
                res.reason = QStringLiteral("Multiple approach charts for runway %1").arg(m_destRwy);
            }
        }
        break;
    }
    case FlightPhase::Landing:
    case FlightPhase::TaxiIn:
    case FlightPhase::Parked: {
        if (m_destIcao.isEmpty()) break;
        QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_destIcao);
        for (const ChartEntry& c : charts) {
            if (c.type == ChartType::AirportDiagram) {
                res.chart = c;
                res.confidence = AssociationConfidence::Exact;
                res.reason = QStringLiteral("Destination Airport Diagram for taxi-in & parking");
                break;
            }
        }
        break;
    }
    default:
        break;
    }

    if (res.confidence == AssociationConfidence::Exact || res.confidence == AssociationConfidence::Likely) {
        emit chartSuggested(res.chart, res.confidence, res.reason);
        if (m_autoMode == ChartAutoMode::AutoOpen && res.confidence == AssociationConfidence::Exact) {
            emit chartAutoOpened(res.chart);
        }
    }

    return res;
}

} // namespace openairac
