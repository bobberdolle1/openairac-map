/*****************************************************************************
* OpenAIRAC Map — Active Flight Workspace Dock & Execution Manager
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
#include <QComboBox>
#include <QDateTime>

namespace atools {
namespace fs {
namespace sc {
class SimConnectData;
}
}
}

namespace openairac {

class ActiveFlightDock : public QDockWidget {
    Q_OBJECT

public:
    explicit ActiveFlightDock(QWidget *parent = nullptr);
    virtual ~ActiveFlightDock() override = default;

    static ActiveFlightDock* instance();

    void setFlightPlan(
        const QString& depIcao,
        const QString& destIcao,
        const QStringList& routeWaypoints,
        const QList<QPair<double, double>>& routeCoordinates,
        double destElevationFt = 0.0,
        double cruiseAltFt = 35000.0
    );

    // Getters for REST API
    QString currentPhaseStr() const;
    QString phaseEvidence() const { return m_evidence; }
    QString activeLegName() const;
    QString nextFixName() const;
    QString prevFixName() const;
    double crossTrackNm() const { return m_currentXtkNm; }
    double distanceToNextFixNm() const { return m_distToNextFixNm; }
    double remainingRouteDistNm() const { return m_remainingRouteDistNm; }
    double directDestDistNm() const { return m_directDestDistNm; }
    int eteNextFixSec() const { return m_eteNextSec; }
    int eteDestSec() const { return m_eteDestSec; }
    double todDistanceNm() const { return m_todDistNm; }
    double requiredVsFpm() const { return m_requiredVsFpm; }
    double profileDeviationFt() const { return m_profileDevFt; }
    bool isOffRoute() const { return qAbs(m_currentXtkNm) > 5.0; }
    bool isTelemetryStale() const;
    bool isSimConnected() const { return m_simConnected; }
    int activeLegIndex() const { return m_activeLegIdx; }

    QJsonObject executionSnapshot() const;

public slots:
    void onSimDataReceived(const atools::fs::sc::SimConnectData& data);
    void onFlightPhaseChanged(openairac::FlightPhase oldPhase, openairac::FlightPhase newPhase, const QString& evidence);
    void onChartSuggested(const openairac::ChartEntry& chart, const QString& confidence, const QString& reason);
    void onOpenSuggestedChartClicked();
    void onDirectToClicked();
    void onMissedApproachClicked();

private:
    static ActiveFlightDock* s_instance;

    QString m_depIcao;
    QString m_destIcao;
    QStringList m_waypoints;
    QList<QPair<double, double>> m_routeCoords;
    double m_destElevationFt = 0.0;
    double m_cruiseAltFt = 35000.0;

    int m_activeLegIdx = 0;
    double m_currentXtkNm = 0.0;
    double m_distToNextFixNm = 0.0;
    double m_remainingRouteDistNm = 0.0;
    double m_directDestDistNm = 0.0;
    int m_eteNextSec = 0;
    int m_eteDestSec = 0;
    double m_todDistNm = 0.0;
    double m_requiredVsFpm = 0.0;
    double m_profileDevFt = 0.0;

    bool m_simConnected = false;
    QDateTime m_lastTelemetryTime;
    double m_lastAltFt = 0.0;
    double m_lastGroundspeedKt = 0.0;
    double m_lastVsFpm = 0.0;

    FlightPhase m_phase = FlightPhase::Preflight;
    QString m_evidence;
    ChartEntry m_suggestedChart;

    // UI
    QLabel *m_connectionBadge = nullptr;
    QLabel *m_phaseBadge = nullptr;
    QLabel *m_evidenceLabel = nullptr;
    QLabel *m_routeLabel = nullptr;
    QLabel *m_activeLegLabel = nullptr;
    QLabel *m_telemetryLabel = nullptr;
    QLabel *m_progressLabel = nullptr;
    QLabel *m_xtkLabel = nullptr;
    QLabel *m_todLabel = nullptr;
    QLabel *m_descentMonitorLabel = nullptr;

    QComboBox *m_directToCombo = nullptr;
    QPushButton *m_directToBtn = nullptr;
    QPushButton *m_missedAppBtn = nullptr;

    QPushButton *m_openChartBtn = nullptr;
    QLabel *m_chartSuggestionLabel = nullptr;

    QTextBrowser *m_destSummaryBrowser = nullptr;

    void updateDisplay();
    void computeNavigationGeometry(double lat, double lon, double gsKt, double altFt, double vsFpm);
};

} // namespace openairac

#endif // OPENAIRAC_ACTIVEFLIGHTDOCK_H
