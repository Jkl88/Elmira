/**
 * as5600_driver.h — магнитный энкодер AS5600 для открытия замка «ключом с магнитом»
 * Проверка начального угла и поворота на 90° по часовой
 */

#ifndef AS5600_DRIVER_H
#define AS5600_DRIVER_H

#include <Arduino.h>

/// Инициализация I2C и AS5600
bool as5600Init();

/// Текущий угол (0..4095 или градусы — см. реализацию)
float as5600GetAngle();

/// Сброс «состояния ключа» (вызвать при смене сценария)
void as5600ResetKeyState();

/// Проверка: магнит в правильной ориентации и выполнен поворот на 90° по часовой. true = можно открыть
bool as5600CheckKeyOpened();

/// Калибровка: сохранить текущий угол как «старт» или «конец» в NVS
bool as5600CalibSaveStart();
bool as5600CalibSaveEnd();
/// Текущие калибровочные углы (из NVS или config по умолчанию)
float as5600GetCalibStartDeg();
float as5600GetCalibEndDeg();

#endif // AS5600_DRIVER_H
