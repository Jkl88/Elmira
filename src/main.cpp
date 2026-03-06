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
#include "power_save.h"
#include "hardware/as5600_driver.h"
#include "hardware/servo_driver.h"

TKWifiManager wifiMgr(80);

static LockState _lastBroadcastState = LOCK_STATE_UNKNOWN;
static float _calibPrevAngle = -1000.0f;

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
    powerSaveSetup();

    Serial.println(F("[Elmira] Готов. Открой http://192.168.4.1 или IP в STA."));
}

void loop() {
    wifiMgr.loop();

    if (lockWebIsCalibMode()) {
        // Режим калибровки: серва следует за поворотом магнитного ключа
        float a = as5600GetAngle();
        if (a >= 0) {
            if (_calibPrevAngle > -500.0f) {
                float d = a - _calibPrevAngle;
                if (d > 10.0f) {
                    servoRun(SERVO_DIR_OPEN);
                } else if (d < -10.0f) {
                    servoRun(SERVO_DIR_CLOSE);
                } else {
                    servoStop();
                }
            }
            _calibPrevAngle = a;
        }
        static uint32_t lastCalib = 0;
        if (millis() - lastCalib >= 200) {
            lastCalib = millis();
            lockWebBroadcastCalibData();
        }
    } else {
        _calibPrevAngle = -1000.0f;
        if (lockGetState() == LOCK_STATE_IN_PROGRESS) {
            powerSaveReportActivity();
        }
        lockStateTick();

        if (as5600CheckKeyOpened()) {
            powerSaveReportActivity();
            lockRequestOpen();
            as5600ResetKeyState();
        }

        LockState cur = lockGetState();
        if (cur != _lastBroadcastState) {
            _lastBroadcastState = cur;
            lockWebBroadcastState();
        }
    }

    powerSaveTick();
    yield();
}
