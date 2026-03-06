/**
 * lock_password.cpp — пароль в NVS, только цифры, 8 символов
 */

#include "lock_password.h"
#include "config.h"
#include <Preferences.h>

static Preferences _prefs;
static String _cached;

static bool onlyDigits(const String& s) {
    if (s.length() != LOCK_PASSWORD_LEN) return false;
    for (unsigned i = 0; i < s.length(); i++)
        if (!isDigit(s[i])) return false;
    return true;
}

void lockPasswordInit() {
    _prefs.begin(NVS_NAMESPACE, false);
    _cached = _prefs.getString(NVS_KEY_PASSWORD, "");
    if (!onlyDigits(_cached)) {
        _cached = LOCK_DEFAULT_PASSWORD;
        _prefs.putString(NVS_KEY_PASSWORD, _cached);
    }
    _prefs.end();
}

bool lockPasswordCheck(const String& input) {
    return input.length() == LOCK_PASSWORD_LEN && onlyDigits(input) && input == _cached;
}

bool lockPasswordSet(const String& newPass) {
    if (!onlyDigits(newPass)) return false;
    _prefs.begin(NVS_NAMESPACE, false);
    _prefs.putString(NVS_KEY_PASSWORD, newPass);
    _prefs.end();
    _cached = newPass;
    return true;
}

String lockPasswordGet() {
    return _cached;
}
