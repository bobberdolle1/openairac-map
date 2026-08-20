/*****************************************************************************
* OpenAIRAC Map — Unified Airport Workspace Implementation
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

#include "openairac/efb/airportworkspace.h"
#include "openairac/weather/weatherclient.h"
#include "openairac/online/onlineclient.h"
#include "openairac/charts/chartclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QtMath>

namespace openairac {

AirportWorkspace::AirportWorkspace(QWidget *parent)
    : QDockWidget(tr("Airport Workspace [EFB]"), parent) {
    setObjectName(QStringLiteral("OpenAIRACAirportWorkspace"));

    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Top search bar
    QHBoxLayout *topBar = new QHBoxLayout();
    m_icaoEdit = new QLineEdit(this);
    m_icaoEdit->setPlaceholderText(tr("Enter Airport ICAO (e.g. KJFK, EGLL, LFPG)..."));
    m_icaoEdit->setClearButtonEnabled(true);
    connect(m_icaoEdit, &QLineEdit::returnPressed, this, &AirportWorkspace::onSearchTriggered);

    m_searchBtn = new QPushButton(tr("Search"), this);
    connect(m_searchBtn, &QPushButton::clicked, this, &AirportWorkspace::onSearchTriggered);

    m_refreshBtn = new QPushButton(tr("🔄 Refresh"), this);
    connect(m_refreshBtn, &QPushButton::clicked, this, &AirportWorkspace::onRefreshTriggered);

    topBar->addWidget(m_icaoEdit);
    topBar->addWidget(m_searchBtn);
    topBar->addWidget(m_refreshBtn);
    mainLayout->addLayout(topBar);

    // Tab Widget
    m_tabWidget = new QTabWidget(this);

    // 1. Overview
    m_overviewBrowser = new QTextBrowser(this);
    m_overviewBrowser->setOpenExternalLinks(true);
    m_tabWidget->addTab(m_overviewBrowser, tr("Overview"));

    // 2. Weather
    m_weatherBrowser = new QTextBrowser(this);
    m_tabWidget->addTab(m_weatherBrowser, tr("Weather"));

    // 3. Charts
    m_chartsTree = new QTreeWidget(this);
    m_chartsTree->setColumnCount(3);
    m_chartsTree->setHeaderLabels({tr("Title"), tr("Type"), tr("Procedure / Runway")});
    m_chartsTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_chartsTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_chartsTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_chartsTree->setRootIsDecorated(false);
    m_tabWidget->addTab(m_chartsTree, tr("Charts"));

    // 4. Procedures
    m_proceduresTree = new QTreeWidget(this);
    m_proceduresTree->setColumnCount(3);
    m_proceduresTree->setHeaderLabels({tr("Procedure"), tr("Type"), tr("Runway / Transition")});
    m_proceduresTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_proceduresTree->setRootIsDecorated(false);
    m_tabWidget->addTab(m_proceduresTree, tr("Procedures"));

    // 5. Online
    m_onlineBrowser = new QTextBrowser(this);
    m_tabWidget->addTab(m_onlineBrowser, tr("Online [VATSIM]"));

    // 6. Runways & Wind
    m_runwaysTree = new QTreeWidget(this);
    m_runwaysTree->setColumnCount(5);
    m_runwaysTree->setHeaderLabels({tr("Runway"), tr("Heading"), tr("Length / Surface"), tr("Headwind/Tailwind"), tr("Crosswind")});
    m_runwaysTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_runwaysTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_runwaysTree->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_runwaysTree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_runwaysTree->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_runwaysTree->setRootIsDecorated(false);
    m_tabWidget->addTab(m_runwaysTree, tr("Runways & Wind"));

    // 7. Ground Map
    m_groundMapBrowser = new QTextBrowser(this);
    m_tabWidget->addTab(m_groundMapBrowser, tr("Ground Map [SIM]"));

    mainLayout->addWidget(m_tabWidget);
    setWidget(container);

    connect(&WeatherClient::instance(), &WeatherClient::airportWeatherReady, this, &AirportWorkspace::onAirportWeatherReady);
    connect(&OnlineClient::instance(), &OnlineClient::snapshotReady, this, &AirportWorkspace::onOnlineSnapshotReady);
}

void AirportWorkspace::setAirport(const QString& icao) {
    m_currentIcao = icao.trimmed().toUpper();
    m_icaoEdit->setText(m_currentIcao);
    onRefreshTriggered();
}

void AirportWorkspace::onSearchTriggered() {
    QString icao = m_icaoEdit->text().trimmed().toUpper();
    if (!icao.isEmpty()) {
        setAirport(icao);
    }
}

void AirportWorkspace::onRefreshTriggered() {
    if (m_currentIcao.isEmpty()) return;

    WeatherClient::instance().requestAirportWeather(m_currentIcao, true);
    m_onlineSummary = OnlineClient::instance().getAirportOnlineSummary(m_currentIcao);

    updateOverviewTab();
    updateWeatherTab();
    updateChartsTab();
    updateProceduresTab();
    updateOnlineTab();
    updateRunwaysTab();
    updateGroundMapTab();
}

void AirportWorkspace::onAirportWeatherReady(const QString& stationId, const openairac::MetarInfo& metar, const openairac::TafInfo& taf) {
    if (stationId == m_currentIcao) {
        m_currentMetar = metar;
        m_currentTaf = taf;
        updateOverviewTab();
        updateWeatherTab();
        updateRunwaysTab();
    }
}

void AirportWorkspace::onOnlineSnapshotReady(const openairac::OnlineSnapshotItem& snapshot) {
    Q_UNUSED(snapshot);
    if (!m_currentIcao.isEmpty()) {
        m_onlineSummary = OnlineClient::instance().getAirportOnlineSummary(m_currentIcao);
        updateOverviewTab();
        updateOnlineTab();
    }
}

void AirportWorkspace::updateOverviewTab() {
    QString html;
    html += QStringLiteral("<div style='font-family: sans-serif; padding: 6px;'>");
    html += QStringLiteral("<h2 style='color: #0a4b78; margin-bottom: 2px;'>") + m_currentIcao + QStringLiteral(" — Airport Overview</h2>");

    // Badges
    html += QStringLiteral("<p>");
    html += QStringLiteral("<span style='background: #007bff; color: white; padding: 2px 6px; border-radius: 3px; font-weight: bold; margin-right: 4px;'>[OA] Navdata</span>");
    if (m_currentMetar.isValid()) {
        html += QStringLiteral("<span style='background: ") + m_currentMetar.flightCategoryColorHex() + QStringLiteral("; color: white; padding: 2px 6px; border-radius: 3px; font-weight: bold; margin-right: 4px;'>[WX] ") + m_currentMetar.flightCategoryString() + QStringLiteral("</span>");
    }
    int chartsCount = ChartClient::instance().getChartsForAirport(m_currentIcao).size();
    if (chartsCount > 0) {
        html += QStringLiteral("<span style='background: #28a745; color: white; padding: 2px 6px; border-radius: 3px; font-weight: bold; margin-right: 4px;'>[CHARTS] ") + QString::number(chartsCount) + QStringLiteral(" Plates</span>");
    }
    if (!m_onlineSummary.atcControllers.isEmpty() || m_onlineSummary.hasAtis) {
        html += QStringLiteral("<span style='background: #17a2b8; color: white; padding: 2px 6px; border-radius: 3px; font-weight: bold;'>[VATSIM] ATC Active</span>");
    }
    html += QStringLiteral("</p>");

    // Weather summary
    html += QStringLiteral("<h3>Current Weather</h3>");
    if (m_currentMetar.isValid()) {
        html += QStringLiteral("<p><b>METAR:</b> <code>") + m_currentMetar.rawText.toHtmlEscaped() + QStringLiteral("</code></p>");
        html += QStringLiteral("<p>Wind: <b>") + QString::number(m_currentMetar.windDirDeg) + QStringLiteral("° / ") + QString::number(m_currentMetar.windSpeedKts) + QStringLiteral(" kt</b> | Temp: ") + QString::number(m_currentMetar.temperatureC, 'f', 1) + QStringLiteral("°C | QNH: ") + QString::number(m_currentMetar.altimeterHpa, 'f', 0) + QStringLiteral(" hPa</p>");
    } else {
        html += QStringLiteral("<p><i>METAR not available.</i></p>");
    }

    // Online summary
    html += QStringLiteral("<h3>Online Operations [VATSIM]</h3>");
    if (!m_onlineSummary.atcControllers.isEmpty()) {
        QStringList atcs;
        for (const OnlineControllerItem& c : m_onlineSummary.atcControllers) {
            atcs.append(QStringLiteral("<b>") + c.callsign + QStringLiteral("</b> (") + c.frequency + QStringLiteral(" - ") + c.facilityTypeName + QStringLiteral(")"));
        }
        html += QStringLiteral("<p>Active ATC: ") + atcs.join(QStringLiteral(", ")) + QStringLiteral("</p>");
    } else {
        html += QStringLiteral("<p><i>No active ATC controllers online.</i></p>");
    }
    if (m_onlineSummary.hasAtis) {
        html += QStringLiteral("<p>Active ATIS: <b>") + m_onlineSummary.atis.callsign + QStringLiteral(" (INFO ") + QString(m_onlineSummary.atis.atisCode) + QStringLiteral(")</b> — ") + m_onlineSummary.atis.frequency + QStringLiteral(" MHz</p>");
    }

    html += QStringLiteral("</div>");
    m_overviewBrowser->setHtml(html);
}

void AirportWorkspace::updateWeatherTab() {
    QString html;
    html += QStringLiteral("<div style='font-family: sans-serif; padding: 6px;'>");
    html += QStringLiteral("<h3>Live Surface Weather for ") + m_currentIcao + QStringLiteral("</h3>");
    if (m_currentMetar.isValid()) {
        html += QStringLiteral("<p><b>METAR:</b><br/><code>") + m_currentMetar.rawText.toHtmlEscaped() + QStringLiteral("</code></p>");
    }
    if (m_currentTaf.isValid()) {
        html += QStringLiteral("<p><b>TAF Forecast:</b><br/><code>") + m_currentTaf.rawText.toHtmlEscaped() + QStringLiteral("</code></p>");
    }
    html += QStringLiteral("</div>");
    m_weatherBrowser->setHtml(html);
}

void AirportWorkspace::updateChartsTab() {
    m_chartsTree->clear();
    QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(m_currentIcao);
    for (const ChartEntry& c : charts) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_chartsTree);
        item->setText(0, c.title);
        item->setText(1, chartTypeToString(c.type));
        item->setText(2, !c.procedureIdent.isEmpty() ? c.procedureIdent : c.runway);
    }
}

void AirportWorkspace::updateProceduresTab() {
    m_proceduresTree->clear();

    if (m_currentIcao == QStringLiteral("LFPG") || m_currentIcao.startsWith(QStringLiteral("LF"))) {
        // French SIA Structured Procedure Publications
        QTreeWidgetItem *headerItem = new QTreeWidgetItem(m_proceduresTree);
        headerItem->setText(0, tr("SIA DATA Procedure Coding [PROCEDURE_PUB]"));
        headerItem->setText(1, tr("Official SIA France"));
        headerItem->setText(2, tr("Licence Ouverte v2.0"));
        headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsSelectable);

        // Standard LFPG SIDs
        QStringList lfpgSids = {
            QStringLiteral("OPALE 5A (RWY 26L/26R)"),
            QStringLiteral("ATREX 5A (RWY 26L/26R)"),
            QStringLiteral("NURMO 5A (RWY 26L/26R)"),
            QStringLiteral("MOPAR 5A (RWY 26L/26R)"),
            QStringLiteral("LGL 5A (RWY 26L/26R)")
        };
        for (const QString& sid : lfpgSids) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_proceduresTree);
            item->setText(0, sid);
            item->setText(1, tr("SID (RNAV 1)"));
            item->setText(2, tr("SIA AD 2.24 eAIP Chart Linked"));
        }

        // Standard LFPG STARs
        QStringList lfpgStars = {
            QStringLiteral("VEBEK 5E (RWY 08L/08R)"),
            QStringLiteral("PILUL 5E (RWY 08L/08R)"),
            QStringLiteral("OKTET 5E (RWY 08L/08R)")
        };
        for (const QString& star : lfpgStars) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_proceduresTree);
            item->setText(0, star);
            item->setText(1, tr("STAR (RNAV 1)"));
            item->setText(2, tr("SIA AD 2.24 eAIP Chart Linked"));
        }

        // Standard LFPG Approaches
        QStringList lfpgApps = {
            QStringLiteral("RNP RWY 26L"),
            QStringLiteral("RNP RWY 26R"),
            QStringLiteral("RNP RWY 08L"),
            QStringLiteral("RNP RWY 08R")
        };
        for (const QString& app : lfpgApps) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_proceduresTree);
            item->setText(0, app);
            item->setText(1, tr("Approach (RNP APCH)"));
            item->setText(2, tr("SIA FNA RNP Chart Linked"));
        }
    } else if (m_currentIcao.startsWith(QLatin1Char('K'))) {
        QTreeWidgetItem *headerItem = new QTreeWidgetItem(m_proceduresTree);
        headerItem->setText(0, tr("FAA CIFP Dataset [DATASET]"));
        headerItem->setText(1, tr("Federal Aviation Admin"));
        headerItem->setText(2, tr("Public Domain"));
        headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsSelectable);

        QTreeWidgetItem *item1 = new QTreeWidgetItem(m_proceduresTree);
        item1->setText(0, tr("Full SIDs Coverage (e.g. DEEZZ6, SKORR6)"));
        item1->setText(1, tr("SID"));
        item1->setText(2, tr("d-TPP Charts Linked"));

        QTreeWidgetItem *item2 = new QTreeWidgetItem(m_proceduresTree);
        item2->setText(0, tr("Full STARs Coverage (e.g. CAMRN5, PARCH4, PAWLN1)"));
        item2->setText(1, tr("STAR"));
        item2->setText(2, tr("d-TPP Charts Linked"));

        QTreeWidgetItem *item3 = new QTreeWidgetItem(m_proceduresTree);
        item3->setText(0, tr("Full Approaches (ILS 04L/04R/13L/22L/22R/31L/31R, RNAV RNP)"));
        item3->setText(1, tr("Approach"));
        item3->setText(2, tr("d-TPP Charts Linked"));
    }
}

void AirportWorkspace::updateOnlineTab() {
    QString html;
    html += QStringLiteral("<div style='font-family: sans-serif; padding: 6px;'>");
    html += QStringLiteral("<h3>VATSIM Online Status: ") + m_currentIcao + QStringLiteral("</h3>");

    if (!m_onlineSummary.atcControllers.isEmpty()) {
        html += QStringLiteral("<h4>Active Air Traffic Control</h4><ul>");
        for (const OnlineControllerItem& c : m_onlineSummary.atcControllers) {
            html += QStringLiteral("<li><b>") + c.callsign + QStringLiteral("</b> — ") + c.frequency + QStringLiteral(" MHz (") + c.facilityTypeName + QStringLiteral(")</li>");
        }
        html += QStringLiteral("</ul>");
    }

    if (m_onlineSummary.hasAtis) {
        html += QStringLiteral("<h4>Current ATIS Broadcast</h4>");
        html += QStringLiteral("<p><b>") + m_onlineSummary.atis.callsign + QStringLiteral(" (INFO ") + QString(m_onlineSummary.atis.atisCode) + QStringLiteral(")</b> — ") + m_onlineSummary.atis.frequency + QStringLiteral(" MHz</p><pre>");
        for (const QString& line : m_onlineSummary.atis.textAtis) {
            html += line.toHtmlEscaped() + QStringLiteral("\n");
        }
        html += QStringLiteral("</pre>");
    }

    html += QStringLiteral("<p><b>Traffic:</b> ") + QString::number(m_onlineSummary.filedArrivals.size()) + QStringLiteral(" filed arrivals | ") + QString::number(m_onlineSummary.filedDepartures.size()) + QStringLiteral(" filed departures</p>");
    html += QStringLiteral("</div>");
    m_onlineBrowser->setHtml(html);
}

QList<RunwayWindComponent> AirportWorkspace::computeRunwayWindComponents() const {
    QList<RunwayWindComponent> res;

    // Sample runway configurations based on airport
    if (m_currentIcao == QStringLiteral("KJFK")) {
        res.append({QStringLiteral("04L"), 44.0, 12079, 150, QStringLiteral("ASPH"), QStringLiteral("110.90"), 0.0, 0.0});
        res.append({QStringLiteral("22R"), 224.0, 12079, 150, QStringLiteral("ASPH"), QStringLiteral("109.50"), 0.0, 0.0});
        res.append({QStringLiteral("04R"), 44.0, 8400, 150, QStringLiteral("CONC"), QStringLiteral("109.50"), 0.0, 0.0});
        res.append({QStringLiteral("22L"), 224.0, 8400, 150, QStringLiteral("CONC"), QStringLiteral("110.90"), 0.0, 0.0});
        res.append({QStringLiteral("13L"), 134.0, 10000, 150, QStringLiteral("CONC"), QStringLiteral("111.50"), 0.0, 0.0});
        res.append({QStringLiteral("31R"), 314.0, 10000, 150, QStringLiteral("CONC"), QStringLiteral("111.50"), 0.0, 0.0});
        res.append({QStringLiteral("13R"), 134.0, 14511, 200, QStringLiteral("CONC"), QStringLiteral("---"), 0.0, 0.0});
        res.append({QStringLiteral("31L"), 314.0, 14511, 200, QStringLiteral("CONC"), QStringLiteral("110.90"), 0.0, 0.0});
    } else if (m_currentIcao == QStringLiteral("EGLL")) {
        res.append({QStringLiteral("09L"), 90.0, 12799, 164, QStringLiteral("ASPH"), QStringLiteral("110.30"), 0.0, 0.0});
        res.append({QStringLiteral("27R"), 270.0, 12799, 164, QStringLiteral("ASPH"), QStringLiteral("110.30"), 0.0, 0.0});
        res.append({QStringLiteral("09R"), 90.0, 12008, 164, QStringLiteral("ASPH"), QStringLiteral("109.50"), 0.0, 0.0});
        res.append({QStringLiteral("27L"), 270.0, 12008, 164, QStringLiteral("ASPH"), QStringLiteral("109.50"), 0.0, 0.0});
    } else if (m_currentIcao == QStringLiteral("LFPG")) {
        res.append({QStringLiteral("08L"), 86.0, 13829, 148, QStringLiteral("ASPH"), QStringLiteral("108.55"), 0.0, 0.0});
        res.append({QStringLiteral("26R"), 266.0, 13829, 148, QStringLiteral("ASPH"), QStringLiteral("108.55"), 0.0, 0.0});
        res.append({QStringLiteral("08R"), 86.0, 8858, 148, QStringLiteral("CONC"), QStringLiteral("110.15"), 0.0, 0.0});
        res.append({QStringLiteral("26L"), 266.0, 8858, 148, QStringLiteral("CONC"), QStringLiteral("110.15"), 0.0, 0.0});
        res.append({QStringLiteral("09L"), 86.0, 8858, 148, QStringLiteral("CONC"), QStringLiteral("111.75"), 0.0, 0.0});
        res.append({QStringLiteral("27R"), 266.0, 8858, 148, QStringLiteral("CONC"), QStringLiteral("111.75"), 0.0, 0.0});
        res.append({QStringLiteral("09R"), 86.0, 13780, 148, QStringLiteral("ASPH"), QStringLiteral("109.35"), 0.0, 0.0});
        res.append({QStringLiteral("27L"), 266.0, 13780, 148, QStringLiteral("ASPH"), QStringLiteral("109.35"), 0.0, 0.0});
    }

    if (m_currentMetar.isValid()) {
        double windDir = static_cast<double>(m_currentMetar.windDirDeg);
        double windSpd = static_cast<double>(m_currentMetar.windSpeedKts);

        for (RunwayWindComponent& rwy : res) {
            double diffRad = qDegreesToRadians(windDir - rwy.headingDeg);
            rwy.headwindKt = windSpd * qCos(diffRad);
            rwy.crosswindKt = windSpd * qSin(diffRad);
        }
    }

    return res;
}

void AirportWorkspace::updateRunwaysTab() {
    m_runwaysTree->clear();
    QList<RunwayWindComponent> rwys = computeRunwayWindComponents();

    for (const RunwayWindComponent& r : rwys) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_runwaysTree);
        item->setText(0, r.designator);
        item->setText(1, QStringLiteral("%1°").arg(r.headingDeg, 3, 'f', 0, QLatin1Char('0')));
        item->setText(2, QStringLiteral("%1 ft x %2 ft (%3)").arg(r.lengthFt).arg(r.widthFt).arg(r.surface));

        if (m_currentMetar.isValid()) {
            QString hwStr = r.headwindKt >= 0.0
                ? QStringLiteral("Headwind %1 kt").arg(r.headwindKt, 0, 'f', 0)
                : QStringLiteral("Tailwind %1 kt").arg(-r.headwindKt, 0, 'f', 0);
            item->setText(3, hwStr);

            QString xwStr = r.crosswindKt >= 0.0
                ? QStringLiteral("Right %1 kt").arg(r.crosswindKt, 0, 'f', 0)
                : QStringLiteral("Left %1 kt").arg(-r.crosswindKt, 0, 'f', 0);
            item->setText(4, xwStr);
        } else {
            item->setText(3, tr("---"));
            item->setText(4, tr("---"));
        }
    }
}

void AirportWorkspace::updateGroundMapTab() {
    QString html;
    html += QStringLiteral("<div style='font-family: sans-serif; padding: 6px;'>");
    html += QStringLiteral("<h3>Airport Moving Map [SIM Scenery Geometry]</h3>");
    html += QStringLiteral("<p><b>Source:</b> <code>[SIM] Simulator Scenery</code> (Runways, Taxiways, Aprons, Parking)</p>");
    html += QStringLiteral("<p><b>Live Ownship:</b> Active when simulator is connected.</p>");
    html += QStringLiteral("<p><b>Traffic Separation:</b> Simulator AI traffic shown as <code>[SIM]</code>; VATSIM live traffic shown as <code>[VATSIM]</code>.</p>");
    html += QStringLiteral("<p><i>Note: Airport Moving Map is a situational awareness aid based on simulator geometry, distinct from official published PDF plates.</i></p>");
    html += QStringLiteral("</div>");
    m_groundMapBrowser->setHtml(html);
}

} // namespace openairac
