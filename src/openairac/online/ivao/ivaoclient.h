/*****************************************************************************
* OpenAIRAC Map — IVAO Online Network Client Interface
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

#ifndef OPENAIRAC_IVAOCLIENT_H
#define OPENAIRAC_IVAOCLIENT_H

#include "openairac/online/onlinenetworkmodel.h"
#include <QObject>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace openairac {

class IvaoClient : public QObject {
    Q_OBJECT

public:
    static IvaoClient& instance();

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    int refreshIntervalSecs() const { return m_refreshIntervalSecs; }
    void setRefreshIntervalSecs(int secs);

    OnlineSnapshotItem latestSnapshot() const;

    AirportOnlineItem getAirportOnlineSummary(const QString& icao) const;
    RouteOnlineItem getRouteOnlineAwareness(
        const QString& depIcao,
        const QString& arrIcao,
        const QList<QPair<double, double>>& routeCoordinates,
        double corridorHalfWidthNm = 50.0
    ) const;

    void requestSnapshot(bool force = false);

signals:
    void snapshotReady(const openairac::OnlineSnapshotItem& snapshot);
    void networkStatusChanged(const QString& status, int connectedClients, int ageSeconds);
    void requestFailed(const QString& errorString);

private slots:
    void onRefreshTimer();

private:
    IvaoClient();
    virtual ~IvaoClient() override = default;

    bool m_enabled = true;
    int m_refreshIntervalSecs = 15;
    QString m_dataUrl = QStringLiteral("https://api.ivao.aero/v2/tracker/whazzup");

    QNetworkAccessManager *m_netManager = nullptr;
    QTimer *m_refreshTimer = nullptr;

    OnlineSnapshotItem m_latestSnapshot;
    QHash<QString, OnlinePilotItem> m_previousPilots;

    void parseIvaoWhazzupJson(const QByteArray& data);
    void parseIvaoWhazzupTxt(const QString& text);
};

} // namespace openairac

#endif // OPENAIRAC_IVAOCLIENT_H
