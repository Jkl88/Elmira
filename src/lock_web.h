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

#endif // LOCK_WEB_H
