/**
 * hall_sensors.h — опрос двух цифровых датчиков холла SS41F (открыто/закрыто)
 */

#ifndef HALL_SENSORS_H
#define HALL_SENSORS_H

#include <Arduino.h>

/// Инициализация пинов датчиков холла (INPUT_PULLUP при необходимости)
void hallSensorsInit();

/// true, если датчик «открыто» активен (низкий уровень при подключении через подтяжку)
bool hallIsOpen();

/// true, если датчик «закрыто» активен
bool hallIsClosed();

/// Состояние по двум датчикам: 0 = закрыто, 1 = открыто, -1 = неизвестно (ни оба не активны, или оба активны)
int hallGetState();

#endif // HALL_SENSORS_H
