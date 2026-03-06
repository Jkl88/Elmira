/**
 * lock_web.cpp — GET /open?pass=..., GET /close, WebSocket-команды и рассылка состояния
 */

#include "lock_web.h"
#include "lock_state.h"
#include "lock_password.h"
#include "config.h"
#include <TKWifiManager.h>
#include <ArduinoJson.h>

static TKWifiManager* _wifiMgr = nullptr;

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

    wifiMgr.addRoute("/close", HTTP_GET, []() {
        if (!lockRequestClose()) {
            _wifiMgr->web().send(200, "application/json", "{\"ok\":false,\"error\":\"invalid_state\"}");
            return;
        }
        _wifiMgr->web().send(200, "application/json", "{\"ok\":true}");
    });

    wifiMgr.setUserWsHook([](uint8_t id, WStype_t type, const uint8_t* payload, size_t len) {
        if (type == WStype_DISCONNECTED) return;
        if (type != WStype_TEXT || !_wifiMgr) return;

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
