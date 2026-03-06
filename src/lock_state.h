/**
 * lock_state.h — состояния замка и логика открытия/закрытия/аварийного закрытия
 */

#ifndef LOCK_STATE_H
#define LOCK_STATE_H

#include <Arduino.h>

// Состояния замка для индикации и логики
enum LockState {
    LOCK_STATE_CLOSED = 0,   // закрыто
    LOCK_STATE_OPEN = 1,     // открыто
    LOCK_STATE_IN_PROGRESS,   // процесс открытия или закрытия
    LOCK_STATE_UNKNOWN       // ни один датчик не активен
};

/// Инициализация (пины и серва уже инициализированы)
void lockStateInit();

/// Вызывать из loop(); обновляет состояние по датчикам и управляет сервой
void lockStateTick();

/// Текущее состояние
LockState lockGetState();

/// Запрос открытия (по паролю или по магнитному ключу). Возвращает true, если команда принята
bool lockRequestOpen();

/// Запрос закрытия
bool lockRequestClose();

/// Аварийное закрытие: медленно крутить до срабатывания датчика «закрыто»
void lockRequestEmergencyClose();

/// Остановка сервы и переход в «неизвестно» (при неопределённости датчиков во время движения)
void lockEmergencyStop();

#endif // LOCK_STATE_H
