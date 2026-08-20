/*****************************************************************************
* OpenAIRAC Map — Settings Migration & Coexistence Isolation Implementation
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

#include "openairac/product/migration.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

namespace openairac {

void MigrationManager::checkAndApplyMigrations() {
    QSettings settings(QStringLiteral("OpenAIRAC"), QStringLiteral("OpenAIRAC Map"));
    QString currentVersion = settings.value(QStringLiteral("setup_version"), QString()).toString();

    if (currentVersion.isEmpty()) {
        // Fresh install or v0.5 upgrade
        settings.setValue(QStringLiteral("setup_version"), QStringLiteral("1.0.0"));
        settings.setValue(QStringLiteral("navigation_provider_default"), QStringLiteral("OpenAIRAC"));
        settings.setValue(QStringLiteral("weather_enabled"), true);
        settings.setValue(QStringLiteral("vatsim_enabled"), true);
    } else if (currentVersion == QStringLiteral("0.5.0")) {
        // Upgrade from v0.5.0 -> v1.0.0 (preserve all user preferences)
        settings.setValue(QStringLiteral("setup_version"), QStringLiteral("1.0.0"));
    }
}

bool MigrationManager::hasPreviousVersionSettings() {
    QSettings settings(QStringLiteral("OpenAIRAC"), QStringLiteral("OpenAIRAC Map"));
    return settings.contains(QStringLiteral("first_run_completed"));
}

void MigrationManager::importLittleNavmapSettings(bool copyFlightPlans) {
    Q_UNUSED(copyFlightPlans);
    // Non-destructive copy from Little Navmap configuration if present
    QSettings lnmSettings(QStringLiteral("ABarthel"), QStringLiteral("little_navmap"));
    if (lnmSettings.allKeys().isEmpty()) {
        return;
    }

    QSettings ourSettings(QStringLiteral("OpenAIRAC"), QStringLiteral("OpenAIRAC Map"));
    // Copy simulator paths if not already set
    if (!ourSettings.contains(QStringLiteral("SimulatorPath")) && lnmSettings.contains(QStringLiteral("SimulatorPath"))) {
        ourSettings.setValue(QStringLiteral("SimulatorPath"), lnmSettings.value(QStringLiteral("SimulatorPath")));
    }
}

} // namespace openairac
