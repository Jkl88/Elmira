/**
 * lock_web.h — HTTP-маршруты и WebSocket для замка (TKWifiManager)
 */

#ifndef LOCK_WEB_H
#define LOCK_WEB_H

class TKWifiManager;

/// Регистрация маршрутов /open, /close и WS-хука. Вызвать после wifiMgr.begin().
void lockWebSetup(TKWifiManager& wifiMgr);

/// Отправить текущее состояние замка всем WS-клиентам (вызывать при изменении состояния).
void lockWebBroadcastState();

/// Режим калибровки: серва от ключа, холлы в UI
bool lockWebIsCalibMode();
void lockWebSetCalibMode(bool on);
/// Рассылка данных калибровки (угол AS5600, холлы) по WebSocket
void lockWebBroadcastCalibData();

#endif // LOCK_WEB_H
