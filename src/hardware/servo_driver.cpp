/**
 * servo_driver.cpp — серва 360°: 1500 = стоп, <1500 одна сторона, >1500 другая
 */

#include "servo_driver.h"
#include "../config.h"
#include <Servo.h>

static Servo _servo;
static int _currentDir = 0;
static uint32_t _startMs = 0;
static const uint16_t US_STOP = 1500;
static const uint16_t US_CW   = 1700;
static const uint16_t US_CCW  = 1300;
static const uint16_t US_CW_SLOW  = 1550;   // медленное вращение
static const uint16_t US_CCW_SLOW = 1450;

void servoInit() {
    _servo.attach(PIN_SERVO);
    servoStop();
}

void servoRun(int direction) {
    _currentDir = direction;
    _startMs = millis();
    if (direction > 0)
        _servo.writeMicroseconds(US_CW);
    else if (direction < 0)
        _servo.writeMicroseconds(US_CCW);
    else
        _servo.writeMicroseconds(US_STOP);
}

void servoRunSlow(int direction) {
    _currentDir = direction;
    _startMs = millis();
    if (direction > 0)
        _servo.writeMicroseconds(US_CW_SLOW);
    else if (direction < 0)
        _servo.writeMicroseconds(US_CCW_SLOW);
    else
        _servo.writeMicroseconds(US_STOP);
}

void servoStop() {
    _currentDir = 0;
    _servo.writeMicroseconds(US_STOP);
}

bool servoCheckTimeout() {
    if (_currentDir == 0) return false;
    return (millis() - _startMs) >= (uint32_t)SERVO_TIMEOUT_MS;
}

void servoResetTimeout() {
    _startMs = millis();
}
