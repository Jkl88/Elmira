/**
 * lock_web.cpp — GET /open?pass=..., GET /close, WebSocket-команды и рассылка состояния
 */

#include "lock_web.h"
#include "lock_state.h"
#include "lock_password.h"
#include "power_save.h"
#include "config.h"
#include "hardware/as5600_driver.h"
#include "hardware/hall_sensors.h"
#include "hardware/servo_driver.h"
#include <TKWifiManager.h>
#include <ArduinoJson.h>

static TKWifiManager* _wifiMgr = nullptr;
static bool _calibMode = false;

static const char* stateStr(LockState s) {
    switch (s) {
        case LOCK_STATE_OPEN:       return "open";
        case LOCK_STATE_CLOSED:     return "closed";
        case LOCK_STATE_IN_PROGRESS: return "in_progress";
        default:                    return "unknown";
    }
}

void lockWebSetup(TKWifiManager& wifiMgr) {
    _wifiMgr = &wifiMgr;

    wifiMgr.addRoute("/open", HTTP_GET, []() {
        powerSaveReportActivity();
        String pass = _wifiMgr->web().arg("pass");
        if (!lockPasswordCheck(pass)) {
            _wifiMgr->web().send(200, "application/json", "{\"ok\":false,\"error\":\"wrong_pass\"}");
            return;
        }
        if (!lockRequestOpen()) {
            _wifiMgr->web().send(200, "application/json", "{\"ok\":false,\"error\":\"invalid_state\"}");
            return;
        }
        _wifiMgr->web().send(200, "application/json", "{\"ok\":true}");
    });

    wifiMgr.addRoute("/status", HTTP_GET, []() {
        powerSaveReportActivity();
        LockState s = lockGetState();
        float a = as5600GetAngle();
        bool ho = hallIsOpen();
        bool hc = hallIsClosed();
        String j = "{\"state\":\"";
        j += stateStr(s);
        j += "\",\"hall_open\":";
        j += ho ? "1" : "0";
        j += ",\"hall_closed\":";
        j += hc ? "1" : "0";
        j += ",\"as5600_angle\":";
        j += (a >= 0) ? String(a, 1) : "null";
        j += ",\"as5600_calib_start\":";
        j += String(as5600GetCalibStartDeg(), 1);
        j += ",\"as5600_calib_end\":";
        j += String(as5600GetCalibEndDeg(), 1);
        j += "}";
        _wifiMgr->web().send(200, "application/json", j);
    });

    wifiMgr.addRoute("/close", HTTP_GET, []() {
        powerSaveReportActivity();
        if (!lockRequestClose()) {
            _wifiMgr->web().send(200, "application/json", "{\"ok\":false,\"error\":\"invalid_state\"}");
            return;
        }
        _wifiMgr->web().send(200, "application/json", "{\"ok\":true}");
    });

    wifiMgr.setUserWsHook([](uint8_t id, WStype_t type, const uint8_t* payload, size_t len) {
        if (type == WStype_DISCONNECTED) return;
        if (type != WStype_TEXT || !_wifiMgr) return;
        powerSaveReportActivity();
        String s((char*)payload, len);

        if (s == "getState") {
            lockWebBroadcastState();
            return;
        }

        if (s == "close") {
            lockRequestClose();
            return;
        }

        if (s == "emergencyClose") {
            lockRequestEmergencyClose();
            return;
        }

        if (s.startsWith("{\"cmd\":\"open\"")) {
            StaticJsonDocument<128> doc;
            if (deserializeJson(doc, s) == DeserializationError::Ok) {
                const char* pass = doc["pass"];
                if (pass && lockPasswordCheck(String(pass))) {
                    lockRequestOpen();
                } else {
                    _wifiMgr->ws().sendTXT(id, "{\"type\":\"lock\",\"ok\":false,\"error\":\"wrong_pass\"}");
                }
            }
            return;
        }

        if (s == "as5600CalibStart") {
            if (as5600CalibSaveStart()) {
                _wifiMgr->ws().sendTXT(id, "{\"type\":\"as5600_calib\",\"ok\":true,\"start\":\"saved\"}");
            } else {
                _wifiMgr->ws().sendTXT(id, "{\"type\":\"as5600_calib\",\"ok\":false,\"error\":\"as5600\"}");
            }
            return;
        }
        if (s == "as5600CalibEnd") {
            if (as5600CalibSaveEnd()) {
                _wifiMgr->ws().sendTXT(id, "{\"type\":\"as5600_calib\",\"ok\":true,\"end\":\"saved\"}");
            } else {
                _wifiMgr->ws().sendTXT(id, "{\"type\":\"as5600_calib\",\"ok\":false,\"error\":\"as5600\"}");
            }
            return;
        }

        if (s == "calibModeOn") {
            servoStop();
            _calibMode = true;
            _wifiMgr->ws().sendTXT(id, "{\"type\":\"calib_mode\",\"on\":true}");
            return;
        }
        if (s == "calibModeOff") {
            _calibMode = false;
            _wifiMgr->ws().sendTXT(id, "{\"type\":\"calib_mode\",\"on\":false}");
            return;
        }
        if (s == "hallCalibOpen") {
            hallCalibSaveAsOpen();
            _wifiMgr->ws().sendTXT(id, "{\"type\":\"hall_calib\",\"ok\":true,\"as\":\"open\"}");
            return;
        }
        if (s == "hallCalibClosed") {
            hallCalibSaveAsClosed();
            _wifiMgr->ws().sendTXT(id, "{\"type\":\"hall_calib\",\"ok\":true,\"as\":\"closed\"}");
            return;
        }

        if (s.startsWith("{\"cmd\":\"setPassword\"")) {
            if (lockGetState() != LOCK_STATE_OPEN) {
                _wifiMgr->ws().sendTXT(id, "{\"type\":\"lock\",\"ok\":false,\"error\":\"lock_must_be_open\"}");
                return;
            }
            StaticJsonDocument<128> doc;
            if (deserializeJson(doc, s) == DeserializationError::Ok) {
                const char* pass = doc["pass"];
                if (pass && lockPasswordSet(String(pass))) {
                    _wifiMgr->ws().sendTXT(id, "{\"type\":\"lock\",\"ok\":true}");
                } else {
                    _wifiMgr->ws().sendTXT(id, "{\"type\":\"lock\",\"ok\":false,\"error\":\"invalid_pass\"}");
                }
            }
            return;
        }

        _wifiMgr->ws().sendTXT(id, "{\"type\":\"error\",\"msg\":\"unknown\"}");
    });
}

void lockWebBroadcastState() {
    if (!_wifiMgr) return;
    LockState s = lockGetState();
    String j = "{\"type\":\"lock\",\"state\":\"";
    j += stateStr(s);
    j += "\"}";
    _wifiMgr->ws().broadcastTXT(j);
}

bool lockWebIsCalibMode() { return _calibMode; }
void lockWebSetCalibMode(bool on) {
    if (on) servoStop();
    _calibMode = on;
}

void lockWebBroadcastCalibData() {
    if (!_wifiMgr || !_calibMode) return;
    uint8_t ho, hc;
    hallGetRaw(&ho, &hc);
    float a = as5600GetAngle();
    String j = "{\"type\":\"calib_data\",\"as5600_angle\":";
    j += (a >= 0) ? String(a, 1) : "null";
    j += ",\"hall_open\":";
    j += (int)ho;
    j += ",\"hall_closed\":";
    j += (int)hc;
    j += "}";
    _wifiMgr->ws().broadcastTXT(j);
}
