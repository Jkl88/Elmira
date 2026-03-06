/**
 * as5600_driver.cpp — AS5600 по I2C, проверка угла и поворота на 90° по часовой
 */

#include "as5600_driver.h"
#include "../config.h"
#include <Wire.h>
#include <Adafruit_AS5600.h>

static Adafruit_AS5600 _as5600;
static bool _inited = false;
static float _angleStart = -1;   // угол в момент «правильной» ориентации
static bool _keyOpened = false;   // флаг: поворот на 90° выполнен

bool as5600Init() {
    Wire.begin(PIN_SDA, PIN_SCL);
    _inited = _as5600.begin();
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

    // Запоминаем угол при первом попадании в стартовый сектор (допуск ±10°)
    const float tol = 10.0f;
    if (_angleStart < 0) {
        if (fabsf(a - (float)AS5600_ANGLE_START_DEG) < tol || fabsf(a - (float)AS5600_ANGLE_START_DEG - 360.0f) < tol)
            _angleStart = a;
        return false;
    }

    // Поворот по часовой: считаем, что значение угла растёт
    float diff = a - _angleStart;
    if (diff < 0) diff += 360.0f;
    float delta = (float)(AS5600_ANGLE_END_DEG - AS5600_ANGLE_START_DEG);
    if (diff >= delta - tol && diff <= delta + tol) {
        _keyOpened = true;
        return true;
    }
    return _keyOpened;
}
