/*****************************************************************************
* OpenAIRAC Map — Automation HTTP API Controller
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

#ifndef OPENAIRAC_APICONTROLLER_H
#define OPENAIRAC_APICONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>

namespace openairac {

struct ApiResponse {
    int statusCode = 200;
    QByteArray contentType = "application/json; charset=utf-8";
    QByteArray body;
    QMap<QByteArray, QByteArray> headers;

    static ApiResponse json(const QJsonObject& obj, int code = 200);
    static ApiResponse error(int code, const QString& message);
};

class OpenAiracApiController : public QObject {
    Q_OBJECT

public:
    static OpenAiracApiController& instance();

    ApiResponse dispatch(const QString& method, const QString& path, const QByteArray& body, const QMap<QString, QString>& queryParams);

private:
    OpenAiracApiController();
    virtual ~OpenAiracApiController() override = default;

    ApiResponse handleGetStatus();
    ApiResponse handleGetAirport(const QString& icao);
    ApiResponse handleGetCoverage(const QString& icao);
    ApiResponse handleGetFlightplan();
    ApiResponse handleGetActiveFlight();
    ApiResponse handleGetSim();
    ApiResponse handleGetExecutionStatus();
    ApiResponse handleGetExecutionActiveLeg();
    ApiResponse handleGetExecutionProgress();
    ApiResponse handleGetExecutionWeather();
    ApiResponse handleGetExecutionSnapshot();
    ApiResponse handleGetWeather(const QString& icao);
    ApiResponse handleGetOnline(const QString& network);
    ApiResponse handleGetCharts(const QString& icao);
    ApiResponse handleGetProcedures(const QString& icao);

    ApiResponse handlePostFlightplan(const QByteArray& body);
    ApiResponse handlePostRandomFlightplan(const QByteArray& body);
    ApiResponse handlePostMapFocus(const QByteArray& body);
    ApiResponse handlePostChartsOpen(const QByteArray& body);
    ApiResponse handlePostSelectProcedure(const QByteArray& body);
};

} // namespace openairac

#endif // OPENAIRAC_APICONTROLLER_H
