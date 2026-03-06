/**
 * power_save.h — энергосбережение при питании от батареи
 * WiFi остаётся включён (modem sleep по умолчанию в STA), GET и веб работают.
 * При простое: задержка в loop, понижение частоты CPU, отключение BT.
 */

#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include <Arduino.h>

/// Вызвать при любой активности (HTTP, WebSocket, движение замка)
void powerSaveReportActivity();

/// Инициализация: отключение BT, при необходимости тип сна WiFi
void powerSaveSetup();

/// Вызывать в конце каждой итерации loop(): задержка и смена частоты CPU при простое
void powerSaveTick();

#endif // POWER_SAVE_H
