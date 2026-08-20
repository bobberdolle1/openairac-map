/*****************************************************************************
* OpenAIRAC Map — Aircraft Profile & Route Suitability
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

#ifndef OPENAIRAC_AIRCRAFTPROFILE_H
#define OPENAIRAC_AIRCRAFTPROFILE_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QList>

namespace openairac {

enum class AircraftClass {
    LightPiston,
    Turboprop,
    RegionalJet,
    NarrowbodyJet,
    WidebodyJet,
    B747Class,
    Custom
};

struct AircraftProfile {
    QString name;
    QString icaoType;
    AircraftClass aircraftClass = AircraftClass::NarrowbodyJet;
    int minRunwayLengthFt = 6000;
    int preferredRunwayLengthFt = 7500;
    int minRunwayWidthFt = 100;
    double minDistanceNm = 250.0;
    double maxDistanceNm = 3200.0;
    double preferredDistanceNm = 800.0;
    bool requiresHardSurface = true;
    bool requiresIfr = true;
    bool requiresTower = false;
    int cruiseSpeedKts = 450;

    static AircraftProfile b747Class();
    static AircraftProfile narrowbodyJet();
    static AircraftProfile regionalJet();
    static AircraftProfile turboprop();
    static AircraftProfile lightPiston();

    static QList<AircraftProfile> defaultProfiles();

    QJsonObject toJson() const;
    static AircraftProfile fromJson(const QJsonObject& json);
    QString className() const;
};

struct RandomFlightPlanResult {
    QString departureIcao;
    QString departureName;
    int departureRunwayLengthFt = 0;
    QString destinationIcao;
    QString destinationName;
    int destinationRunwayLengthFt = 0;
    double distanceNm = 0.0;
    int estimatedEnrouteMinutes = 0;
    AircraftProfile profile;
    quint64 seed = 0;
    QStringList suitabilityNotes;

    QJsonObject toJson() const;
    static RandomFlightPlanResult fromJson(const QJsonObject& json);
};

} // namespace openairac

#endif // OPENAIRAC_AIRCRAFTPROFILE_H
