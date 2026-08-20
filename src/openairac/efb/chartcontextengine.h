/*****************************************************************************
* OpenAIRAC Map — Phase-Aware Chart Context & Suggestion Engine
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

#ifndef OPENAIRAC_CHARTCONTEXTENGINE_H
#define OPENAIRAC_CHARTCONTEXTENGINE_H

#include "openairac/charts/chartmodel.h"
#include "openairac/efb/flightphase.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QSet>

namespace openairac {

enum class ChartAutoMode {
    Off,
    Suggest,
    AutoOpen
};

struct ContextualChartResult {
    ChartEntry chart;
    QString confidence;
    QString reason;
    bool isAmbiguous = false;
    QList<ChartEntry> ambiguousCandidates;
};

class ChartContextEngine : public QObject {
    Q_OBJECT

public:
    static ChartContextEngine& instance();

    ChartAutoMode autoMode() const { return m_autoMode; }
    void setAutoMode(ChartAutoMode mode) { m_autoMode = mode; }

    void setFlightPlanContext(
        const QString& depIcao,
        const QString& destIcao,
        const QString& sidName = QString(),
        const QString& starName = QString(),
        const QString& appName = QString(),
        const QString& depRwy = QString(),
        const QString& destRwy = QString()
    );

    ContextualChartResult evaluateContext(FlightPhase phase);

    // Pinned charts preservation
    void pinChart(const ChartEntry& chart);
    void unpinChart(const QString& chartId);
    bool isPinned(const QString& chartId) const;
    QList<ChartEntry> pinnedCharts() const { return m_pinnedCharts; }

signals:
    void chartSuggested(const openairac::ChartEntry& chart, const QString& confidence, const QString& reason);
    void chartAutoOpened(const openairac::ChartEntry& chart);
private:
    ChartContextEngine();
    virtual ~ChartContextEngine() override = default;

    ChartAutoMode m_autoMode = ChartAutoMode::Suggest;

    QString m_depIcao;
    QString m_destIcao;
    QString m_sidName;
    QString m_starName;
    QString m_appName;
    QString m_depRwy;
    QString m_destRwy;

    QList<ChartEntry> m_pinnedCharts;
    QSet<QString> m_pinnedIds;
};

} // namespace openairac

#endif // OPENAIRAC_CHARTCONTEXTENGINE_H
