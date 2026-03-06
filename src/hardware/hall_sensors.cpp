/**
 * hall_sensors.cpp — опрос датчиков холла SS41F, калибровка в NVS (ожидаемые значения открыто/закрыто)
 */

#include "hall_sensors.h"
#include "../config.h"
#include <Preferences.h>

// Ожидаемые значения пинов (0=LOW, 1=HIGH) в положении «открыто» и «закрыто»
static uint8_t _open_ho = 0, _open_hc = 1;   // по умолчанию: открыто = пин open LOW, closed HIGH
static uint8_t _closed_ho = 1, _closed_hc = 0;

static void loadCalibFromNVS() {
    Preferences p;
    p.begin(NVS_NAMESPACE, true);
    _open_ho   = p.getUChar(NVS_KEY_HALL_OPEN_OPEN, 0);     // пин «открыто» в положении открыто
    _open_hc   = p.getUChar(NVS_KEY_HALL_CLOSED_OPEN, 1);   // пин «закрыто» в положении открыто
    _closed_ho = p.getUChar(NVS_KEY_HALL_OPEN_CLOSED, 1);   // пин «открыто» в положении закрыто
    _closed_hc = p.getUChar(NVS_KEY_HALL_CLOSED_CLOSED, 0); // пин «закрыто» в положении закрыто
    p.end();
}

void hallSensorsInit() {
    pinMode(PIN_HALL_OPEN, INPUT_PULLUP);
    pinMode(PIN_HALL_CLOSED, INPUT_PULLUP);
    loadCalibFromNVS();
}

bool hallIsOpen() {
    return digitalRead(PIN_HALL_OPEN) == LOW;
}

bool hallIsClosed() {
    return digitalRead(PIN_HALL_CLOSED) == LOW;
}

/// Сырые значения (0 или 1) для калибровки и отображения
void hallGetRaw(uint8_t* openPin, uint8_t* closedPin) {
    *openPin  = (digitalRead(PIN_HALL_OPEN) == LOW) ? 0 : 1;
    *closedPin = (digitalRead(PIN_HALL_CLOSED) == LOW) ? 0 : 1;
}

/// Сохранить текущие значения как «положение открыто» / «положение закрыто»
void hallCalibSaveAsOpen() {
    uint8_t o, c;
    hallGetRaw(&o, &c);
    _open_ho = o; _open_hc = c;
    Preferences p;
    p.begin(NVS_NAMESPACE, false);
    p.putUChar(NVS_KEY_HALL_OPEN_OPEN, _open_ho);
    p.putUChar(NVS_KEY_HALL_CLOSED_OPEN, _open_hc);
    p.end();
}
void hallCalibSaveAsClosed() {
    uint8_t o, c;
    hallGetRaw(&o, &c);
    _closed_ho = o; _closed_hc = c;
    Preferences p;
    p.begin(NVS_NAMESPACE, false);
    p.putUChar(NVS_KEY_HALL_OPEN_CLOSED, _closed_ho);
    p.putUChar(NVS_KEY_HALL_CLOSED_CLOSED, _closed_hc);
    p.end();
}

// 0 = закрыто, 1 = открыто, -1 = неизвестно
int hallGetState() {
    uint8_t o, c;
    hallGetRaw(&o, &c);
    if (o == _open_ho && c == _open_hc) return 1;
    if (o == _closed_ho && c == _closed_hc) return 0;
    return -1;
}
