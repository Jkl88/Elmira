/**
 * hall_sensors.cpp — реализация опроса датчиков холла SS41F
 * SS41F: цифровой выход, активный низкий при наличии поля (подтяжка к VCC, выход в GPIO)
 */

#include "hall_sensors.h"
#include "../config.h"

void hallSensorsInit() {
    pinMode(PIN_HALL_OPEN, INPUT_PULLUP);
    pinMode(PIN_HALL_CLOSED, INPUT_PULLUP);
}

bool hallIsOpen() {
    return digitalRead(PIN_HALL_OPEN) == LOW;
}

bool hallIsClosed() {
    return digitalRead(PIN_HALL_CLOSED) == LOW;
}

// 0 = закрыто, 1 = открыто, -1 = неизвестно
int hallGetState() {
    bool open = hallIsOpen();
    bool closed = hallIsClosed();
    if (closed && !open) return 0;
    if (open && !closed) return 1;
    return -1;
}
