/*****************************************************************************
* OpenAIRAC Map — Active Flight Workspace Dock
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

#ifndef OPENAIRAC_ACTIVEFLIGHTDOCK_H
#define OPENAIRAC_ACTIVEFLIGHTDOCK_H

#include "openairac/efb/flightphase.h"
#include "openairac/efb/chartcontextengine.h"
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTextBrowser>
#include <QListWidget>

namespace openairac {

class ActiveFlightDock : public QDockWidget {
    Q_OBJECT

public:
    explicit ActiveFlightDock(QWidget *parent = nullptr);
    virtual ~ActiveFlightDock() override = default;

    void setFlightPlan(
        const QString& depIcao,
        const QString& destIcao,
        const QStringList& routeWaypoints,
        const QList<QPair<double, double>>& routeCoordinates,
        double destElevationFt = 0.0
    );

public slots:
    void onFlightPhaseChanged(openairac::FlightPhase oldPhase, openairac::FlightPhase newPhase, const QString& evidence);
    void onChartSuggested(const openairac::ChartEntry& chart, openairac::AssociationConfidence confidence, const QString& reason);
    void onOpenSuggestedChartClicked();

private:
    QString m_depIcao;
    QString m_destIcao;
    QStringList m_waypoints;
    QList<QPair<double, double>> m_routeCoords;
    double m_destElevationFt = 0.0;

    FlightPhase m_phase = FlightPhase::Preflight;
    QString m_evidence;
    ChartEntry m_suggestedChart;

    // UI
    QLabel *m_phaseBadge = nullptr;
    QLabel *m_evidenceLabel = nullptr;
    QLabel *m_routeLabel = nullptr;
    QLabel *m_progressLabel = nullptr;
    QLabel *m_xtkLabel = nullptr;
    QLabel *m_todLabel = nullptr;

    QPushButton *m_openChartBtn = nullptr;
    QLabel *m_chartSuggestionLabel = nullptr;

    QTextBrowser *m_destSummaryBrowser = nullptr;

    void updateDisplay();
};

} // namespace openairac

#endif // OPENAIRAC_ACTIVEFLIGHTDOCK_H
