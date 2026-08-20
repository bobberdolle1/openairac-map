/*****************************************************************************
* OpenAIRAC Map — Application Update Checker
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

#ifndef OPENAIRAC_APPUPDATER_H
#define OPENAIRAC_APPUPDATER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace openairac {

struct AppUpdateInfo {
    bool hasUpdate = false;
    QString currentVersion = QStringLiteral("1.0.0");
    QString latestVersion;
    QString releaseNotesUrl;
    QString downloadUrl;
    QString sha256Checksum;
};

class AppUpdater : public QObject {
    Q_OBJECT

public:
    static AppUpdater& instance();

    QString currentVersion() const { return QStringLiteral("1.0.0"); }
    QString releaseChannel() const { return m_channel; }
    void setReleaseChannel(const QString& channel) { m_channel = channel; }

    void checkForUpdates(bool silent = false);

signals:
    void updateCheckCompleted(const openairac::AppUpdateInfo& info, bool silent);
    void updateCheckFailed(const QString& errorString, bool silent);

private:
    AppUpdater();
    virtual ~AppUpdater() override = default;

    QString m_channel = QStringLiteral("Stable");
    QNetworkAccessManager *m_netManager = nullptr;
    QString m_releasesApiUrl = QStringLiteral("https://api.github.com/repos/bobberdolle1/openairac-map/releases/latest");
};

} // namespace openairac

#endif // OPENAIRAC_APPUPDATER_H
