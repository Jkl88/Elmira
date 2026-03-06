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

/// Состояние по двум датчикам: 0 = закрыто, 1 = открыто, -1 = неизвестно
int hallGetState();

/// Сырые значения пинов (0=LOW, 1=HIGH) для калибровки
void hallGetRaw(uint8_t* openPin, uint8_t* closedPin);
/// Сохранить текущие значения как положение «открыто» / «закрыто»
void hallCalibSaveAsOpen();
void hallCalibSaveAsClosed();

#endif // HALL_SENSORS_H
