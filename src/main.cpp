/**
 * main.cpp — точка входа Elmira: электронный замок на ESP32-C3
 * Arduino framework: setup/loop, TKWifiManager, логика замка, магнитный ключ AS5600
 */

#define TKWM_USE_LITTLEFS 1
#include <Arduino.h>
#include <TKWifiManager.h>

#include "config.h"
#include "lock_state.h"
#include "lock_password.h"
#include "lock_web.h"
#include "hardware/as5600_driver.h"

TKWifiManager wifiMgr(80);

static LockState _lastBroadcastState = LOCK_STATE_UNKNOWN;

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println(F("\n[Elmira] Электронный замок"));

    lockPasswordInit();
    lockStateInit();

    if (as5600Init())
        Serial.println(F("[Elmira] AS5600 OK"));
    else
        Serial.println(F("[Elmira] AS5600 не найден (I2C)"));

    wifiMgr.begin("Elmira");
    lockWebSetup(wifiMgr);

    Serial.println(F("[Elmira] Готов. Открой http://192.168.4.1 или IP в STA."));
}

void loop() {
    wifiMgr.loop();
    lockStateTick();

    // Магнитный ключ: при правильном повороте AS5600 — открыть
    if (as5600CheckKeyOpened()) {
        lockRequestOpen();
        as5600ResetKeyState();
    }

    // Рассылка состояния по WebSocket при изменении
    LockState cur = lockGetState();
    if (cur != _lastBroadcastState) {
        _lastBroadcastState = cur;
        lockWebBroadcastState();
    }

    // Энергосбережение: при питании от батареи здесь можно добавить
    // light sleep или понижение частоты CPU при простое (см. .apprules).
    yield();
}
