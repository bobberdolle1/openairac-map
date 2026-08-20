/*****************************************************************************
* OpenAIRAC Map — Unified Airport Workspace
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

#ifndef OPENAIRAC_AIRPORTWORKSPACE_H
#define OPENAIRAC_AIRPORTWORKSPACE_H

#include "openairac/weather/weathermodel.h"
#include "openairac/online/onlinenetworkmodel.h"
#include "openairac/charts/chartmodel.h"
#include <QDockWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QLabel>

namespace openairac {

struct RunwayWindComponent {
    QString designator;
    double headingDeg = 0.0;
    int lengthFt = 0;
    int widthFt = 0;
    QString surface;
    QString ilsFrequency;
    double headwindKt = 0.0; // Positive = Headwind, Negative = Tailwind
    double crosswindKt = 0.0; // Positive = Right crosswind, Negative = Left crosswind
};

class AirportWorkspace : public QDockWidget {
    Q_OBJECT

public:
    explicit AirportWorkspace(QWidget *parent = nullptr);
    virtual ~AirportWorkspace() override = default;

    void setAirport(const QString& icao);
    QString currentAirport() const { return m_currentIcao; }

public slots:
    void onSearchTriggered();
    void onRefreshTriggered();

private slots:
    void onAirportWeatherReady(const QString& stationId, const openairac::MetarInfo& metar, const openairac::TafInfo& taf);
    void onOnlineSnapshotReady(const openairac::OnlineSnapshotItem& snapshot);

private:
    QString m_currentIcao;
    MetarInfo m_currentMetar;
    TafInfo m_currentTaf;
    AirportOnlineItem m_onlineSummary;

    // UI
    QLineEdit *m_icaoEdit = nullptr;
    QPushButton *m_searchBtn = nullptr;
    QPushButton *m_refreshBtn = nullptr;
    QTabWidget *m_tabWidget = nullptr;

    // Subpanels
    QTextBrowser *m_overviewBrowser = nullptr;
    QTextBrowser *m_weatherBrowser = nullptr;
    QTreeWidget *m_chartsTree = nullptr;
    QTreeWidget *m_proceduresTree = nullptr;
    QTextBrowser *m_onlineBrowser = nullptr;
    QTreeWidget *m_runwaysTree = nullptr;
    QTextBrowser *m_groundMapBrowser = nullptr;

    void updateOverviewTab();
    void updateWeatherTab();
    void updateChartsTab();
    void updateProceduresTab();
    void updateOnlineTab();
    void updateRunwaysTab();
    void updateGroundMapTab();

    QList<RunwayWindComponent> computeRunwayWindComponents() const;
};

} // namespace openairac

#endif // OPENAIRAC_AIRPORTWORKSPACE_H
