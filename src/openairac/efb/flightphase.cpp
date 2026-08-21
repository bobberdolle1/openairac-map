/*****************************************************************************
* OpenAIRAC Map — Flight Phase Automation Engine Implementation
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

#include "openairac/efb/flightphase.h"
#include <QtMath>

namespace openairac {

QString flightPhaseToString(FlightPhase phase) {
    switch (phase) {
    case FlightPhase::Preflight: return QStringLiteral("PREFLIGHT");
    case FlightPhase::TaxiOut: return QStringLiteral("TAXI_OUT");
    case FlightPhase::Takeoff: return QStringLiteral("TAKEOFF");
    case FlightPhase::InitialClimb: return QStringLiteral("INITIAL_CLIMB");
    case FlightPhase::Departure: return QStringLiteral("DEPARTURE");
    case FlightPhase::Climb: return QStringLiteral("CLIMB");
    case FlightPhase::Cruise: return QStringLiteral("CRUISE");
    case FlightPhase::Descent: return QStringLiteral("DESCENT");
    case FlightPhase::Arrival: return QStringLiteral("ARRIVAL");
    case FlightPhase::Approach: return QStringLiteral("APPROACH");
    case FlightPhase::Final: return QStringLiteral("FINAL");
    case FlightPhase::Landing: return QStringLiteral("LANDING");
    case FlightPhase::TaxiIn: return QStringLiteral("TAXI_IN");
    case FlightPhase::Parked: return QStringLiteral("PARKED");
    case FlightPhase::GoAround: return QStringLiteral("GO_AROUND");
    case FlightPhase::Unknown: return QStringLiteral("UNKNOWN");
    }
    return QStringLiteral("UNKNOWN");
}

QString flightPhaseDisplayString(FlightPhase phase) {
    switch (phase) {
    case FlightPhase::Preflight: return QStringLiteral("Preflight (Gate / Ramp)");
    case FlightPhase::TaxiOut: return QStringLiteral("Taxi Out");
    case FlightPhase::Takeoff: return QStringLiteral("Takeoff Roll");
    case FlightPhase::InitialClimb: return QStringLiteral("Initial Climb");
    case FlightPhase::Departure: return QStringLiteral("Departure (SID)");
    case FlightPhase::Climb: return QStringLiteral("Enroute Climb");
    case FlightPhase::Cruise: return QStringLiteral("Enroute Cruise");
    case FlightPhase::Descent: return QStringLiteral("Enroute Descent");
    case FlightPhase::Arrival: return QStringLiteral("Terminal Arrival (STAR)");
    case FlightPhase::Approach: return QStringLiteral("Instrument Approach");
    case FlightPhase::Final: return QStringLiteral("Final Approach Segment");
    case FlightPhase::Landing: return QStringLiteral("Landing Rollout");
    case FlightPhase::TaxiIn: return QStringLiteral("Taxi In to Gate");
    case FlightPhase::Parked: return QStringLiteral("Parked at Stand");
    case FlightPhase::GoAround: return QStringLiteral("Go-Around / Missed Approach");
    case FlightPhase::Unknown: return QStringLiteral("Flight Phase Unknown");
    }
    return QStringLiteral("Flight Phase Unknown");
}

FlightPhaseEngine& FlightPhaseEngine::instance() {
    static FlightPhaseEngine inst;
    return inst;
}

FlightPhaseEngine::FlightPhaseEngine()
    : QObject(nullptr) {
    m_currentEvidence = QStringLiteral("Initial preflight state");
}

bool FlightPhaseEngine::isAirborne() const {
    switch (m_currentPhase) {
    case FlightPhase::Takeoff:
    case FlightPhase::InitialClimb:
    case FlightPhase::Departure:
    case FlightPhase::Climb:
    case FlightPhase::Cruise:
    case FlightPhase::Descent:
    case FlightPhase::Arrival:
    case FlightPhase::Approach:
    case FlightPhase::Final:
        return true;
    default:
        return false;
    }
}

bool FlightPhaseEngine::isTerminalArrival() const {
    return m_currentPhase == FlightPhase::Arrival
        || m_currentPhase == FlightPhase::Approach
        || m_currentPhase == FlightPhase::Final
        || m_currentPhase == FlightPhase::Landing;
}

void FlightPhaseEngine::reset(FlightPhase initialPhase) {
    FlightPhase old = m_currentPhase;
    m_currentPhase = initialPhase;
    m_consecutiveTicks = 0;
    m_hasBeenAirborne = false;
    m_currentEvidence = QStringLiteral("Engine reset to ") + flightPhaseToString(initialPhase);
    emit phaseChanged(old, m_currentPhase, m_currentEvidence);
}

void FlightPhaseEngine::processTelemetry(const TelemetrySample& sample) {
    // 1. Slew / Teleport Detection
    if (m_lastTelemetry.timestamp.isValid()) {
        qint64 dtMs = m_lastTelemetry.timestamp.msecsTo(sample.timestamp);
        if (dtMs > 0 && dtMs < 5000) {
            double altJump = qAbs(sample.altitudeMslFt - m_lastTelemetry.altitudeMslFt);
            if (altJump > 10000.0) {
                FlightPhase old = m_currentPhase;
                m_currentPhase = sample.onGround ? FlightPhase::Preflight : FlightPhase::Cruise;
                m_consecutiveTicks = 0;
                m_currentEvidence = QStringLiteral("Teleport/Slew detected (Altitude jump %1 ft in %2s); state reset")
                                        .arg(altJump, 0, 'f', 0)
                                        .arg(dtMs / 1000.0, 0, 'f', 1);
                m_lastTelemetry = sample;
                emit phaseChanged(old, m_currentPhase, m_currentEvidence);
                return;
            }
        }
    }

    QString evidence;
    FlightPhase candidate = inferRawPhase(sample, evidence);

    if (!sample.onGround && sample.groundspeedKt > 50.0) {
        m_hasBeenAirborne = true;
    }

    // 2. Hysteresis & Immediate Transitions
    if (candidate == m_currentPhase) {
        m_consecutiveTicks++;
        m_currentEvidence = evidence;
    } else {
        bool immediate = (sample.onGround && m_currentPhase == FlightPhase::Final)
                      || (!sample.onGround && (m_currentPhase == FlightPhase::Takeoff || m_currentPhase == FlightPhase::Preflight));

        if (immediate || m_consecutiveTicks >= 2) {
            FlightPhase old = m_currentPhase;
            m_currentPhase = candidate;
            m_consecutiveTicks = 1;
            m_currentEvidence = evidence;
            emit phaseChanged(old, m_currentPhase, m_currentEvidence);
        } else {
            m_consecutiveTicks++;
        }
    }

    m_lastTelemetry = sample;
}

FlightPhase FlightPhaseEngine::inferRawPhase(const TelemetrySample& telem, QString& evidenceOut) const {
    if (telem.onGround) {
        if (!m_hasBeenAirborne) {
            if (telem.groundspeedKt < 3.0) {
                evidenceOut = QStringLiteral("On ground, stationary (GS < 3 kt)");
                return FlightPhase::Preflight;
            } else if (telem.groundspeedKt < 45.0) {
                evidenceOut = QStringLiteral("On ground, taxiing out (GS %1 kt)").arg(telem.groundspeedKt, 0, 'f', 0);
                return FlightPhase::TaxiOut;
            } else {
                evidenceOut = QStringLiteral("On ground, takeoff roll (GS %1 kt)").arg(telem.groundspeedKt, 0, 'f', 0);
                return FlightPhase::Takeoff;
            }
        } else {
            if (telem.groundspeedKt > 45.0) {
                evidenceOut = QStringLiteral("Touchdown rollout (GS %1 kt)").arg(telem.groundspeedKt, 0, 'f', 0);
                return FlightPhase::Landing;
            } else if (telem.groundspeedKt > 3.0) {
                evidenceOut = QStringLiteral("On ground, taxiing in (GS %1 kt)").arg(telem.groundspeedKt, 0, 'f', 0);
                return FlightPhase::TaxiIn;
            } else {
                evidenceOut = QStringLiteral("Parked at destination stand (GS < 3 kt)");
                return FlightPhase::Parked;
            }
        }
    } else {
        double agl = telem.altitudeAglFt > 0.0 ? telem.altitudeAglFt : telem.altitudeMslFt;
        QChar proc = telem.activeProcedureKind;

        if (agl < 1500.0 && telem.verticalSpeedFpm > 300.0 && (m_currentPhase == FlightPhase::Takeoff || m_currentPhase == FlightPhase::InitialClimb || !m_hasBeenAirborne)) {
            evidenceOut = QStringLiteral("Airborne, climbing rapidly (VS %1 fpm, AGL %2 ft)").arg(telem.verticalSpeedFpm, 0, 'f', 0).arg(agl, 0, 'f', 0);
            return FlightPhase::InitialClimb;
        } else if (proc == QLatin1Char('D') || (telem.distanceFromDepNm < 30.0 && telem.verticalSpeedFpm > 200.0)) {
            evidenceOut = QStringLiteral("Flying SID / Terminal Departure phase");
            return FlightPhase::Departure;
        } else if (proc == QLatin1Char('F') || (telem.distanceToDestNm < 15.0 && agl < 4000.0 && telem.verticalSpeedFpm < -100.0)) {
            if (telem.distanceToDestNm < 5.0 && agl < 1500.0) {
                evidenceOut = QStringLiteral("On final approach segment (Dist %1 NM, AGL %2 ft)").arg(telem.distanceToDestNm, 0, 'f', 1).arg(agl, 0, 'f', 0);
                return FlightPhase::Final;
            } else {
                evidenceOut = QStringLiteral("On instrument approach procedure (Dist %1 NM)").arg(telem.distanceToDestNm, 0, 'f', 1);
                return FlightPhase::Approach;
            }
        } else if (proc == QLatin1Char('E') || (telem.distanceToDestNm < 60.0 && telem.verticalSpeedFpm < -200.0)) {
            evidenceOut = QStringLiteral("Terminal Arrival (STAR) / Descent towards destination (Dist %1 NM)").arg(telem.distanceToDestNm, 0, 'f', 1);
            return FlightPhase::Arrival;
        } else if (telem.verticalSpeedFpm < -300.0 && telem.distanceToDestNm < 150.0) {
            evidenceOut = QStringLiteral("Enroute descent (VS %1 fpm, Dist %2 NM)").arg(telem.verticalSpeedFpm, 0, 'f', 0).arg(telem.distanceToDestNm, 0, 'f', 0);
            return FlightPhase::Descent;
        } else if (telem.verticalSpeedFpm > 300.0) {
            evidenceOut = QStringLiteral("Enroute climb (VS %1 fpm, Alt %2 ft)").arg(telem.verticalSpeedFpm, 0, 'f', 0).arg(telem.altitudeMslFt, 0, 'f', 0);
            return FlightPhase::Climb;
        } else {
            evidenceOut = QStringLiteral("Enroute cruise (Alt %1 ft, GS %2 kt)").arg(telem.altitudeMslFt, 0, 'f', 0).arg(telem.groundspeedKt, 0, 'f', 0);
            return FlightPhase::Cruise;
        }
    }
}

} // namespace openairac
