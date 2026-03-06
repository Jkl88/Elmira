/**
 * lock_state.cpp — машина состояний замка: открыт/закрыт/в процессе/неизвестно
 */

#include "lock_state.h"
#include "config.h"
#include "hardware/hall_sensors.h"
#include "hardware/servo_driver.h"

static LockState _state = LOCK_STATE_UNKNOWN;
static enum { IDLE, OPENING, CLOSING, EMERGENCY_CLOSING } _action = IDLE;

void lockStateInit() {
    hallSensorsInit();
    servoInit();
    int h = hallGetState();
    if (h == 0) _state = LOCK_STATE_CLOSED;
    else if (h == 1) _state = LOCK_STATE_OPEN;
    else _state = LOCK_STATE_UNKNOWN;
    _action = IDLE;
}

void lockStateTick() {
    int hall = hallGetState();

    switch (_action) {
    case OPENING:
        if (hall == 1) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_OPEN;
        } else if (hall == -1) {
            lockEmergencyStop();
        } else if (servoCheckTimeout()) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_UNKNOWN;
        }
        break;

    case CLOSING:
        if (hall == 0) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_CLOSED;
        } else if (hall == -1) {
            lockEmergencyStop();
        } else if (servoCheckTimeout()) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_UNKNOWN;
        }
        break;

    case EMERGENCY_CLOSING:
        if (hall == 0) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_CLOSED;
        } else if (hall == -1) {
            servoStop();
            _action = IDLE;
            _state = LOCK_STATE_UNKNOWN;
        }
        break;

    case IDLE:
    default:
        if (hall == 0) _state = LOCK_STATE_CLOSED;
        else if (hall == 1) _state = LOCK_STATE_OPEN;
        else _state = LOCK_STATE_UNKNOWN;
        break;
    }
}

LockState lockGetState() {
    if (_action != IDLE) return LOCK_STATE_IN_PROGRESS;
    return _state;
}

bool lockRequestOpen() {
    if (_action != IDLE) return false;
    int hall = hallGetState();
    if (hall == 1) return true;  // уже открыто
    if (hall == -1) return false; // ошибка: неизвестно
    _action = OPENING;
    _state = LOCK_STATE_IN_PROGRESS;
    servoResetTimeout();
    servoRun(SERVO_DIR_OPEN);
    return true;
}

bool lockRequestClose() {
    if (_action != IDLE) return false;
    int hall = hallGetState();
    if (hall == 0) return true;  // уже закрыто
    if (hall == -1) return false;
    _action = CLOSING;
    _state = LOCK_STATE_IN_PROGRESS;
    servoResetTimeout();
    servoRun(SERVO_DIR_CLOSE);
    return true;
}

void lockRequestEmergencyClose() {
    if (_action != IDLE) {
        servoStop();
        _action = IDLE;
    }
    int hall = hallGetState();
    if (hall == 0) {
        _state = LOCK_STATE_CLOSED;
        return;
    }
    _action = EMERGENCY_CLOSING;
    _state = LOCK_STATE_IN_PROGRESS;
    servoRunSlow(SERVO_DIR_CLOSE);
}

void lockEmergencyStop() {
    servoStop();
    _action = IDLE;
    _state = LOCK_STATE_UNKNOWN;
}
