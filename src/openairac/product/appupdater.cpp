/*****************************************************************************
* OpenAIRAC Map — Application Update Checker Implementation
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

#include "openairac/product/appupdater.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>

namespace openairac {

AppUpdater& AppUpdater::instance() {
    static AppUpdater inst;
    return inst;
}

AppUpdater::AppUpdater()
    : QObject(nullptr) {
    m_netManager = new QNetworkAccessManager(this);
}

void AppUpdater::checkForUpdates(bool silent) {
    QNetworkRequest req(QUrl(m_releasesApiUrl));
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("OpenAIRAC-Map-Updater/1.0.0"));

    QNetworkReply *reply = m_netManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, silent]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit updateCheckFailed(reply->errorString(), silent);
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isObject()) {
            emit updateCheckFailed(QStringLiteral("Invalid release metadata response"), silent);
            return;
        }

        QJsonObject root = doc.object();
        QString tag = root[QStringLiteral("tag_name")].toString().trimmed();
        QString latestVer = tag.startsWith(QLatin1Char('v')) ? tag.mid(1) : tag;

        AppUpdateInfo info;
        info.currentVersion = currentVersion();
        info.latestVersion = latestVer.isEmpty() ? currentVersion() : latestVer;
        info.releaseNotesUrl = root[QStringLiteral("html_url")].toString();

        // Check if latest version > current version
        if (!latestVer.isEmpty() && latestVer != currentVersion()) {
            info.hasUpdate = true;
        }

        QJsonArray assets = root[QStringLiteral("assets")].toArray();
        for (const QJsonValue& val : assets) {
            QJsonObject a = val.toObject();
            QString name = a[QStringLiteral("name")].toString();
            if (name.contains(QStringLiteral("Setup.exe")) || name.contains(QStringLiteral("AppImage"))) {
                info.downloadUrl = a[QStringLiteral("browser_download_url")].toString();
                break;
            }
        }

        emit updateCheckCompleted(info, silent);
    });
}

} // namespace openairac
