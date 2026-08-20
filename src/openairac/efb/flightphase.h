/*****************************************************************************
* OpenAIRAC Map — Flight Phase Automation Engine
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

#ifndef OPENAIRAC_FLIGHTPHASE_H
#define OPENAIRAC_FLIGHTPHASE_H

#include <QObject>
#include <QString>
#include <QDateTime>

namespace openairac {

enum class FlightPhase {
    Preflight,
    TaxiOut,
    Takeoff,
    InitialClimb,
    Departure,
    Climb,
    Cruise,
    Descent,
    Arrival,
    Approach,
    Final,
    Landing,
    TaxiIn,
    Parked,
    Unknown
};

QString flightPhaseToString(FlightPhase phase);
QString flightPhaseDisplayString(FlightPhase phase);

struct TelemetrySample {
    bool onGround = true;
    double altitudeMslFt = 0.0;
    double altitudeAglFt = 0.0;
    double groundspeedKt = 0.0;
    double verticalSpeedFpm = 0.0;
    double distanceToDestNm = 999.0;
    double distanceFromDepNm = 0.0;
    QChar activeProcedureKind = QChar(); // 'D'=SID, 'E'=STAR, 'F'=Approach
    QDateTime timestamp;
};

class FlightPhaseEngine : public QObject {
    Q_OBJECT

public:
    static FlightPhaseEngine& instance();

    FlightPhase currentPhase() const { return m_currentPhase; }
    QString currentEvidence() const { return m_currentEvidence; }
    bool isAirborne() const;
    bool isTerminalArrival() const;

    void processTelemetry(const TelemetrySample& sample);
    void reset(FlightPhase initialPhase = FlightPhase::Preflight);

signals:
    void phaseChanged(openairac::FlightPhase oldPhase, openairac::FlightPhase newPhase, const QString& evidence);

private:
    FlightPhaseEngine();
    virtual ~FlightPhaseEngine() override = default;

    FlightPhase m_currentPhase = FlightPhase::Preflight;
    QString m_currentEvidence;
    int m_consecutiveTicks = 0;
    bool m_hasBeenAirborne = false;
    TelemetrySample m_lastTelemetry;

    FlightPhase inferRawPhase(const TelemetrySample& telem, QString& evidenceOut) const;
};

} // namespace openairac

#endif // OPENAIRAC_FLIGHTPHASE_H
