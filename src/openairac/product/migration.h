/*****************************************************************************
* OpenAIRAC Map — Settings Migration & Coexistence Isolation
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

#ifndef OPENAIRAC_MIGRATION_H
#define OPENAIRAC_MIGRATION_H

#include <QString>

namespace openairac {

class MigrationManager {
public:
    static void checkAndApplyMigrations();
    static bool hasPreviousVersionSettings();
    static void importLittleNavmapSettings(bool copyFlightPlans = true);
};

} // namespace openairac

#endif // OPENAIRAC_MIGRATION_H
