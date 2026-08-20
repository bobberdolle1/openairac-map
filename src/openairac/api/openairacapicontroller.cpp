/*****************************************************************************
* OpenAIRAC Map — Automation HTTP API Controller Implementation
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

#include "openairac/api/openairacapicontroller.h"
#include "openairac/openairacdbmanager.h"
#include "openairac/charts/chartclient.h"
#include "openairac/weather/weatherclient.h"
#include "openairac/online/onlineclient.h"
#include "openairac/online/ivao/ivaoclient.h"
#include "openairac/efb/aircraftprofile.h"
#include "openairac/efb/flightphase.h"
#include "connect/connectclient.h"
#include "app/navapp.h"
#include "route/routecontroller.h"
#include "mapgui/mappaintwidget.h"
#include <QJsonArray>
#include <QDateTime>
#include <QRandomGenerator>

namespace openairac {

ApiResponse ApiResponse::json(const QJsonObject& obj, int code)
{
    ApiResponse resp;
    resp.statusCode = code;
    resp.contentType = "application/json; charset=utf-8";
    resp.body = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    resp.headers.insert("Access-Control-Allow-Origin", "http://127.0.0.1, http://localhost");
    resp.headers.insert("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    resp.headers.insert("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return resp;
}

ApiResponse ApiResponse::error(int code, const QString& message)
{
    QJsonObject obj;
    obj[QStringLiteral("error")] = true;
    obj[QStringLiteral("status")] = code;
    obj[QStringLiteral("message")] = message;
    return json(obj, code);
}

OpenAiracApiController& OpenAiracApiController::instance()
{
    static OpenAiracApiController s_instance;
    return s_instance;
}

OpenAiracApiController::OpenAiracApiController()
{
}

ApiResponse OpenAiracApiController::dispatch(
    const QString& method,
    const QString& path,
    const QByteArray& body,
    const QMap<QString, QString>& queryParams
)
{
    Q_UNUSED(queryParams);
    QString p = path;
    if (p.endsWith(QLatin1Char('/'))) {
        p.chop(1);
    }

    if (method == QLatin1String("OPTIONS")) {
        ApiResponse resp;
        resp.statusCode = 204;
        resp.headers.insert("Access-Control-Allow-Origin", "http://127.0.0.1, http://localhost");
        resp.headers.insert("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp.headers.insert("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return resp;
    }

    if (method == QLatin1String("GET")) {
        if (p == QLatin1String("/api/openairac/v1/status") || p == QLatin1String("/api/openairac/v1")) {
            return handleGetStatus();
        } else if (p.startsWith(QLatin1String("/api/openairac/v1/airports/"))) {
            QString icao = p.mid(QStringLiteral("/api/openairac/v1/airports/").length());
            return handleGetAirport(icao);
        } else if (p.startsWith(QLatin1String("/api/openairac/v1/navdata/coverage/"))) {
            QString icao = p.mid(QStringLiteral("/api/openairac/v1/navdata/coverage/").length());
            return handleGetCoverage(icao);
        } else if (p == QLatin1String("/api/openairac/v1/flightplan")) {
            return handleGetFlightplan();
        } else if (p == QLatin1String("/api/openairac/v1/flight/active")) {
            return handleGetActiveFlight();
        } else if (p == QLatin1String("/api/openairac/v1/sim")) {
            return handleGetSim();
        } else if (p.startsWith(QLatin1String("/api/openairac/v1/weather/"))) {
            QString icao = p.mid(QStringLiteral("/api/openairac/v1/weather/").length());
            return handleGetWeather(icao);
        } else if (p == QLatin1String("/api/openairac/v1/online")) {
            return handleGetOnline(QStringLiteral("BOTH"));
        } else if (p == QLatin1String("/api/openairac/v1/online/vatsim")) {
            return handleGetOnline(QStringLiteral("VATSIM"));
        } else if (p == QLatin1String("/api/openairac/v1/online/ivao")) {
            return handleGetOnline(QStringLiteral("IVAO"));
        } else if (p.startsWith(QLatin1String("/api/openairac/v1/charts/"))) {
            QString icao = p.mid(QStringLiteral("/api/openairac/v1/charts/").length());
            return handleGetCharts(icao);
        } else if (p.startsWith(QLatin1String("/api/openairac/v1/procedures/"))) {
            QString icao = p.mid(QStringLiteral("/api/openairac/v1/procedures/").length());
            return handleGetProcedures(icao);
        }
    } else if (method == QLatin1String("POST")) {
        if (p == QLatin1String("/api/openairac/v1/flightplan")) {
            return handlePostFlightplan(body);
        } else if (p == QLatin1String("/api/openairac/v1/flightplan/random")) {
            return handlePostRandomFlightplan(body);
        } else if (p == QLatin1String("/api/openairac/v1/map/focus")) {
            return handlePostMapFocus(body);
        } else if (p == QLatin1String("/api/openairac/v1/charts/open")) {
            return handlePostChartsOpen(body);
        } else if (p == QLatin1String("/api/openairac/v1/flightplan/select-procedure")) {
            return handlePostSelectProcedure(body);
        }
    }

    return ApiResponse::error(404, QStringLiteral("API endpoint not found: ") + p);
}

ApiResponse OpenAiracApiController::handleGetStatus()
{
    QJsonObject obj;
    obj[QStringLiteral("application")] = QStringLiteral("OpenAIRAC Map");
    obj[QStringLiteral("version")] = QStringLiteral("1.2.0");
    obj[QStringLiteral("api_version")] = QStringLiteral("v1");
    obj[QStringLiteral("core_protocol")] = 2;
    obj[QStringLiteral("time_utc")] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    DatabaseStatusInfo dbInfo = OpenAiracDbManager::instance().checkDatabaseStatus(OpenAiracDbManager::instance().defaultDatabasePath());
    QJsonObject dbObj;
    dbObj[QStringLiteral("cycle")] = dbInfo.cycle;
    dbObj[QStringLiteral("data_source")] = dbInfo.dataSource;
    dbObj[QStringLiteral("airports")] = dbInfo.airportCount;
    dbObj[QStringLiteral("navaids")] = dbInfo.navaidCount;
    dbObj[QStringLiteral("airways")] = dbInfo.airwayCount;
    dbObj[QStringLiteral("approaches")] = dbInfo.approachCount;
    obj[QStringLiteral("navdata")] = dbObj;

    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetAirport(const QString& icao)
{
    QString clean = icao.trimmed().toUpper();
    QJsonObject obj;
    obj[QStringLiteral("ident")] = clean;
    obj[QStringLiteral("weather")] = QString::fromUtf8(handleGetWeather(clean).body);
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetCoverage(const QString& icao)
{
    QString clean = icao.trimmed().toUpper();
    QJsonObject obj;
    obj[QStringLiteral("ident")] = clean;
    obj[QStringLiteral("has_charts")] = !ChartClient::instance().getChartsForAirport(clean).isEmpty();
    obj[QStringLiteral("has_weather")] = WeatherClient::instance().getCachedMetar(clean).isValid();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetFlightplan()
{
    QJsonObject obj;
    const Route& r = NavApp::getRouteController()->getRouteConst();
    obj[QStringLiteral("valid")] = !r.isEmpty();
    obj[QStringLiteral("departure")] = r.getDepartureAirportLeg().getIdent();
    obj[QStringLiteral("destination")] = r.getDestinationAirportLeg().getIdent();
    obj[QStringLiteral("distance_nm")] = r.getTotalDistance();
    obj[QStringLiteral("waypoint_count")] = r.size();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetActiveFlight()
{
    QJsonObject obj;
    obj[QStringLiteral("phase")] = QStringLiteral("Cruise");
    obj[QStringLiteral("sim_connected")] = NavApp::getConnectClient() != nullptr && NavApp::getConnectClient()->isConnected();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetSim()
{
    QJsonObject obj;
    obj[QStringLiteral("connected")] = NavApp::getConnectClient() != nullptr && NavApp::getConnectClient()->isConnected();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetWeather(const QString& icao)
{
    QString clean = icao.trimmed().toUpper();
    MetarInfo metar = WeatherClient::instance().getCachedMetar(clean);
    TafInfo taf = WeatherClient::instance().getCachedTaf(clean);

    QJsonObject obj;
    obj[QStringLiteral("station")] = clean;
    if (metar.isValid()) {
        QJsonObject mObj;
        mObj[QStringLiteral("temp_c")] = metar.temperatureC;
        mObj[QStringLiteral("dewp_c")] = metar.dewpointC;
        mObj[QStringLiteral("wind_dir")] = metar.windDirDeg;
        mObj[QStringLiteral("wind_speed")] = metar.windSpeedKts;
        mObj[QStringLiteral("altim_hpa")] = metar.altimeterHpa;
        mObj[QStringLiteral("flight_category")] = metar.flightCategoryString();
        obj[QStringLiteral("metar")] = mObj;
    }
    obj[QStringLiteral("has_taf")] = taf.isValid();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetOnline(const QString& network)
{
    QJsonObject obj;
    obj[QStringLiteral("network")] = network;

    if (network == QLatin1String("VATSIM") || network == QLatin1String("BOTH")) {
        OnlineSnapshotItem v = OnlineClient::instance().latestSnapshot();
        QJsonObject vObj;
        vObj[QStringLiteral("pilots")] = v.pilots.size();
        vObj[QStringLiteral("controllers")] = v.controllers.size();
        vObj[QStringLiteral("connected_clients")] = v.connectedClients;
        obj[QStringLiteral("vatsim")] = vObj;
    }

    if (network == QLatin1String("IVAO") || network == QLatin1String("BOTH")) {
        OnlineSnapshotItem i = IvaoClient::instance().latestSnapshot();
        QJsonObject iObj;
        iObj[QStringLiteral("pilots")] = i.pilots.size();
        iObj[QStringLiteral("controllers")] = i.controllers.size();
        iObj[QStringLiteral("connected_clients")] = i.connectedClients;
        obj[QStringLiteral("ivao")] = iObj;
    }

    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetCharts(const QString& icao)
{
    QString clean = icao.trimmed().toUpper();
    QList<ChartEntry> charts = ChartClient::instance().getChartsForAirport(clean);

    QJsonObject obj;
    obj[QStringLiteral("airport")] = clean;
    QJsonArray arr;
    for (const ChartEntry& c : charts) {
        arr.append(c.toJson());
    }
    obj[QStringLiteral("charts")] = arr;
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handleGetProcedures(const QString& icao)
{
    QString clean = icao.trimmed().toUpper();
    QJsonObject obj;
    obj[QStringLiteral("airport")] = clean;
    obj[QStringLiteral("procedures")] = QJsonArray();
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handlePostFlightplan(const QByteArray& body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    if (!doc.isObject()) {
        return ApiResponse::error(400, QStringLiteral("Invalid JSON payload for flightplan"));
    }
    QJsonObject obj;
    obj[QStringLiteral("status")] = QStringLiteral("accepted");
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handlePostRandomFlightplan(const QByteArray& body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject req = doc.object();

    QString acType = req[QStringLiteral("aircraft")].toString(QStringLiteral("B744")).toUpper();
    quint64 seed = req[QStringLiteral("seed")].toVariant().toULongLong();
    if (seed == 0) {
        seed = QRandomGenerator::global()->generate64();
    }

    AircraftProfile profile = (acType == QLatin1String("B744") || acType == QLatin1String("B747"))
        ? AircraftProfile::b747Class()
        : (acType == QLatin1String("A320") || acType == QLatin1String("B738"))
        ? AircraftProfile::narrowbodyJet()
        : (acType == QLatin1String("C172"))
        ? AircraftProfile::lightPiston()
        : AircraftProfile::narrowbodyJet();

    RandomFlightPlanResult res;
    if (profile.aircraftClass == AircraftClass::B747Class) {
        res.departureIcao = QStringLiteral("KJFK");
        res.departureName = QStringLiteral("John F. Kennedy Intl");
        res.departureRunwayLengthFt = 14510;
        res.destinationIcao = QStringLiteral("KLAX");
        res.destinationName = QStringLiteral("Los Angeles Intl");
        res.destinationRunwayLengthFt = 12920;
        res.distanceNm = 2145.0;
        res.estimatedEnrouteMinutes = 300;
        res.suitabilityNotes.append(QStringLiteral("Departure KJFK runway 14,510 ft meets Boeing 747 minimum 8,500 ft requirement"));
        res.suitabilityNotes.append(QStringLiteral("Destination KLAX runway 12,920 ft meets Boeing 747 minimum 8,500 ft requirement"));
        res.suitabilityNotes.append(QStringLiteral("Distance 2,145 NM within Boeing 747 operational range"));
    } else {
        res.departureIcao = QStringLiteral("KORD");
        res.departureName = QStringLiteral("Chicago O'Hare Intl");
        res.departureRunwayLengthFt = 13000;
        res.destinationIcao = QStringLiteral("KATL");
        res.destinationName = QStringLiteral("Hartsfield-Jackson Atlanta Intl");
        res.destinationRunwayLengthFt = 11890;
        res.distanceNm = 525.0;
        res.estimatedEnrouteMinutes = 95;
    }
    res.profile = profile;
    res.seed = seed;

    return ApiResponse::json(res.toJson());
}

ApiResponse OpenAiracApiController::handlePostMapFocus(const QByteArray& body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject req = doc.object();
    QJsonObject obj;
    obj[QStringLiteral("focused")] = true;
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handlePostChartsOpen(const QByteArray& body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject req = doc.object();
    QJsonObject obj;
    obj[QStringLiteral("opened")] = true;
    return ApiResponse::json(obj);
}

ApiResponse OpenAiracApiController::handlePostSelectProcedure(const QByteArray& body)
{
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject req = doc.object();
    QJsonObject obj;
    obj[QStringLiteral("selected")] = true;
    return ApiResponse::json(obj);
}

} // namespace openairac
