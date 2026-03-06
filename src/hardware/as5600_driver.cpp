/**
 * as5600_driver.cpp — AS5600 по I2C, проверка угла и поворота на 90° по часовой, калибровка в NVS
 */

#include "as5600_driver.h"
#include "../config.h"
#include <Wire.h>
#include <Adafruit_AS5600.h>
#include <Preferences.h>

static Adafruit_AS5600 _as5600;
static bool _inited = false;
static float _angleStart = -1;
static bool _keyOpened = false;
static float _calibStartDeg = (float)AS5600_ANGLE_START_DEG;
static float _calibEndDeg = (float)AS5600_ANGLE_END_DEG;

static void loadCalibFromNVS() {
    Preferences p;
    p.begin(NVS_NAMESPACE, true);
    float s = p.getFloat(NVS_KEY_AS5600_START, (float)AS5600_ANGLE_START_DEG);
    float e = p.getFloat(NVS_KEY_AS5600_END, (float)AS5600_ANGLE_END_DEG);
    p.end();
    _calibStartDeg = s;
    _calibEndDeg = e;
}

bool as5600Init() {
    Wire.begin(PIN_SDA, PIN_SCL);
    _inited = _as5600.begin();
    loadCalibFromNVS();
    return _inited;
}

float as5600GetAngle() {
    if (!_inited) return -1;
    return _as5600.getAngle();  // градусы 0..360 (Adafruit AS5600)
}

void as5600ResetKeyState() {
    _angleStart = -1;
    _keyOpened = false;
}

bool as5600CheckKeyOpened() {
    if (!_inited) return false;
    float a = as5600GetAngle();
    if (a < 0) return false;

    const float tol = 10.0f;
    if (_angleStart < 0) {
        if (fabsf(a - _calibStartDeg) < tol || fabsf(a - _calibStartDeg - 360.0f) < tol)
            _angleStart = a;
        return false;
    }

    float diff = a - _angleStart;
    if (diff < 0) diff += 360.0f;
    float delta = _calibEndDeg - _calibStartDeg;
    if (delta < 0) delta += 360.0f;
    if (diff >= delta - tol && diff <= delta + tol) {
        _keyOpened = true;
        return true;
    }
    return _keyOpened;
}

bool as5600CalibSaveStart() {
    if (!_inited) return false;
    float a = as5600GetAngle();
    if (a < 0) return false;
    _calibStartDeg = a;
    Preferences p;
    p.begin(NVS_NAMESPACE, false);
    p.putFloat(NVS_KEY_AS5600_START, _calibStartDeg);
    p.end();
    return true;
}

bool as5600CalibSaveEnd() {
    if (!_inited) return false;
    float a = as5600GetAngle();
    if (a < 0) return false;
    _calibEndDeg = a;
    Preferences p;
    p.begin(NVS_NAMESPACE, false);
    p.putFloat(NVS_KEY_AS5600_END, _calibEndDeg);
    p.end();
    return true;
}

float as5600GetCalibStartDeg() { return _calibStartDeg; }
float as5600GetCalibEndDeg()   { return _calibEndDeg; }
