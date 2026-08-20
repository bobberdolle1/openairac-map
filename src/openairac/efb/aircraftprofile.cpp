/*****************************************************************************
* OpenAIRAC Map — Aircraft Profile Implementation
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

#include "openairac/efb/aircraftprofile.h"
#include <QJsonArray>

namespace openairac {

AircraftProfile AircraftProfile::b747Class()
{
    AircraftProfile p;
    p.name = QStringLiteral("Boeing 747-400 / Heavy Widebody");
    p.icaoType = QStringLiteral("B744");
    p.aircraftClass = AircraftClass::B747Class;
    p.minRunwayLengthFt = 8500;
    p.preferredRunwayLengthFt = 10000;
    p.minRunwayWidthFt = 150;
    p.minDistanceNm = 1000.0;
    p.maxDistanceNm = 7500.0;
    p.preferredDistanceNm = 3500.0;
    p.requiresHardSurface = true;
    p.requiresIfr = true;
    p.requiresTower = true;
    p.cruiseSpeedKts = 490;
    return p;
}

AircraftProfile AircraftProfile::narrowbodyJet()
{
    AircraftProfile p;
    p.name = QStringLiteral("Airbus A320 / Boeing 737-800");
    p.icaoType = QStringLiteral("A320");
    p.aircraftClass = AircraftClass::NarrowbodyJet;
    p.minRunwayLengthFt = 6000;
    p.preferredRunwayLengthFt = 7500;
    p.minRunwayWidthFt = 100;
    p.minDistanceNm = 250.0;
    p.maxDistanceNm = 3200.0;
    p.preferredDistanceNm = 800.0;
    p.requiresHardSurface = true;
    p.requiresIfr = true;
    p.requiresTower = false;
    p.cruiseSpeedKts = 450;
    return p;
}

AircraftProfile AircraftProfile::regionalJet()
{
    AircraftProfile p;
    p.name = QStringLiteral("Embraer E190 / Bombardier CRJ-900");
    p.icaoType = QStringLiteral("E190");
    p.aircraftClass = AircraftClass::RegionalJet;
    p.minRunwayLengthFt = 5000;
    p.preferredRunwayLengthFt = 6500;
    p.minRunwayWidthFt = 90;
    p.minDistanceNm = 150.0;
    p.maxDistanceNm = 1800.0;
    p.preferredDistanceNm = 500.0;
    p.requiresHardSurface = true;
    p.requiresIfr = true;
    p.requiresTower = false;
    p.cruiseSpeedKts = 420;
    return p;
}

AircraftProfile AircraftProfile::turboprop()
{
    AircraftProfile p;
    p.name = QStringLiteral("Beechcraft King Air 350 / Dash 8");
    p.icaoType = QStringLiteral("B350");
    p.aircraftClass = AircraftClass::Turboprop;
    p.minRunwayLengthFt = 3500;
    p.preferredRunwayLengthFt = 4500;
    p.minRunwayWidthFt = 75;
    p.minDistanceNm = 80.0;
    p.maxDistanceNm = 1200.0;
    p.preferredDistanceNm = 300.0;
    p.requiresHardSurface = false;
    p.requiresIfr = false;
    p.requiresTower = false;
    p.cruiseSpeedKts = 280;
    return p;
}

AircraftProfile AircraftProfile::lightPiston()
{
    AircraftProfile p;
    p.name = QStringLiteral("Cessna 172 Skyhawk / Piper PA-28");
    p.icaoType = QStringLiteral("C172");
    p.aircraftClass = AircraftClass::LightPiston;
    p.minRunwayLengthFt = 1800;
    p.preferredRunwayLengthFt = 2500;
    p.minRunwayWidthFt = 50;
    p.minDistanceNm = 30.0;
    p.maxDistanceNm = 500.0;
    p.preferredDistanceNm = 120.0;
    p.requiresHardSurface = false;
    p.requiresIfr = false;
    p.requiresTower = false;
    p.cruiseSpeedKts = 120;
    return p;
}

QList<AircraftProfile> AircraftProfile::defaultProfiles()
{
    return { b747Class(), narrowbodyJet(), regionalJet(), turboprop(), lightPiston() };
}

QString AircraftProfile::className() const
{
    switch (aircraftClass) {
        case AircraftClass::LightPiston: return QStringLiteral("Light Piston");
        case AircraftClass::Turboprop: return QStringLiteral("Turboprop");
        case AircraftClass::RegionalJet: return QStringLiteral("Regional Jet");
        case AircraftClass::NarrowbodyJet: return QStringLiteral("Narrowbody Jet");
        case AircraftClass::WidebodyJet: return QStringLiteral("Widebody Jet");
        case AircraftClass::B747Class: return QStringLiteral("Boeing 747 Class");
        case AircraftClass::Custom: return QStringLiteral("Custom");
    }
    return QStringLiteral("Unknown");
}

QJsonObject AircraftProfile::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("name")] = name;
    obj[QStringLiteral("icao_type")] = icaoType;
    obj[QStringLiteral("class")] = className();
    obj[QStringLiteral("min_runway_length_ft")] = minRunwayLengthFt;
    obj[QStringLiteral("preferred_runway_length_ft")] = preferredRunwayLengthFt;
    obj[QStringLiteral("min_runway_width_ft")] = minRunwayWidthFt;
    obj[QStringLiteral("min_distance_nm")] = minDistanceNm;
    obj[QStringLiteral("max_distance_nm")] = maxDistanceNm;
    obj[QStringLiteral("preferred_distance_nm")] = preferredDistanceNm;
    obj[QStringLiteral("requires_hard_surface")] = requiresHardSurface;
    obj[QStringLiteral("requires_ifr")] = requiresIfr;
    obj[QStringLiteral("requires_tower")] = requiresTower;
    obj[QStringLiteral("cruise_speed_kts")] = cruiseSpeedKts;
    return obj;
}

AircraftProfile AircraftProfile::fromJson(const QJsonObject& json)
{
    AircraftProfile p;
    p.name = json[QStringLiteral("name")].toString(p.name);
    p.icaoType = json[QStringLiteral("icao_type")].toString(p.icaoType);
    p.minRunwayLengthFt = json[QStringLiteral("min_runway_length_ft")].toInt(p.minRunwayLengthFt);
    p.preferredRunwayLengthFt = json[QStringLiteral("preferred_runway_length_ft")].toInt(p.preferredRunwayLengthFt);
    p.minRunwayWidthFt = json[QStringLiteral("min_runway_width_ft")].toInt(p.minRunwayWidthFt);
    p.minDistanceNm = json[QStringLiteral("min_distance_nm")].toDouble(p.minDistanceNm);
    p.maxDistanceNm = json[QStringLiteral("max_distance_nm")].toDouble(p.maxDistanceNm);
    p.preferredDistanceNm = json[QStringLiteral("preferred_distance_nm")].toDouble(p.preferredDistanceNm);
    p.requiresHardSurface = json[QStringLiteral("requires_hard_surface")].toBool(p.requiresHardSurface);
    p.requiresIfr = json[QStringLiteral("requires_ifr")].toBool(p.requiresIfr);
    p.requiresTower = json[QStringLiteral("requires_tower")].toBool(p.requiresTower);
    p.cruiseSpeedKts = json[QStringLiteral("cruise_speed_kts")].toInt(p.cruiseSpeedKts);
    return p;
}

QJsonObject RandomFlightPlanResult::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("departure_icao")] = departureIcao;
    obj[QStringLiteral("departure_name")] = departureName;
    obj[QStringLiteral("departure_longest_runway_ft")] = departureRunwayLengthFt;
    obj[QStringLiteral("destination_icao")] = destinationIcao;
    obj[QStringLiteral("destination_name")] = destinationName;
    obj[QStringLiteral("destination_longest_runway_ft")] = destinationRunwayLengthFt;
    obj[QStringLiteral("great_circle_distance_nm")] = distanceNm;
    obj[QStringLiteral("estimated_time_enroute_minutes")] = estimatedEnrouteMinutes;
    obj[QStringLiteral("aircraft_profile")] = profile.toJson();
    obj[QStringLiteral("seed_used")] = static_cast<qint64>(seed);

    QJsonArray notes;
    for (const QString& n : suitabilityNotes) {
        notes.append(n);
    }
    obj[QStringLiteral("suitability_notes")] = notes;
    return obj;
}

RandomFlightPlanResult RandomFlightPlanResult::fromJson(const QJsonObject& json)
{
    RandomFlightPlanResult r;
    r.departureIcao = json[QStringLiteral("departure_icao")].toString();
    r.departureName = json[QStringLiteral("departure_name")].toString();
    r.departureRunwayLengthFt = json[QStringLiteral("departure_longest_runway_ft")].toInt();
    r.destinationIcao = json[QStringLiteral("destination_icao")].toString();
    r.destinationName = json[QStringLiteral("destination_name")].toString();
    r.destinationRunwayLengthFt = json[QStringLiteral("destination_longest_runway_ft")].toInt();
    r.distanceNm = json[QStringLiteral("great_circle_distance_nm")].toDouble();
    r.estimatedEnrouteMinutes = json[QStringLiteral("estimated_time_enroute_minutes")].toInt();
    r.profile = AircraftProfile::fromJson(json[QStringLiteral("aircraft_profile")].toObject());
    r.seed = static_cast<quint64>(json[QStringLiteral("seed_used")].toVariant().toULongLong());

    QJsonArray notes = json[QStringLiteral("suitability_notes")].toArray();
    for (const QJsonValue& v : notes) {
        r.suitabilityNotes.append(v.toString());
    }
    return r;
}

} // namespace openairac
