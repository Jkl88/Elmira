/**
 * buzzer.h — звуковая обратная связь (пищалка)
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

/// Инициализация пина пищалки (вызов при PIN_BUZZER > 0)
void buzzerInit();

/// Короткий сигнал успеха (открыто/закрыто)
void buzzerSuccess();

/// Сигнал ошибки (два коротких)
void buzzerError();

#endif // BUZZER_H
