/**
 * servo_driver.h — управление сервой 360° (непрерывное вращение до срабатывания датчика)
 */

#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Arduino.h>

/// Инициализация сервы (привязка к пину)
void servoInit();

/// Запуск вращения: direction 1 или -1 (см. SERVO_DIR_OPEN / SERVO_DIR_CLOSE)
void servoRun(int direction);

/// Медленное вращение (для аварийного закрытия)
void servoRunSlow(int direction);

/// Остановка сервы
void servoStop();

/// Вызывать в loop(); возвращает true, если таймаут истёк (нужно остановить и сообщить об ошибке)
bool servoCheckTimeout();

/// Сброс таймера таймаута (вызывать при старте движения)
void servoResetTimeout();

#endif // SERVO_DRIVER_H
