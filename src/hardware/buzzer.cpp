/**
 * buzzer.cpp — пищалка: success — один короткий, error — два коротких
 */

#include "buzzer.h"
#include "../config.h"

#if !defined(PIN_BUZZER) || (PIN_BUZZER == 0)
void buzzerInit() {}
void buzzerSuccess() {}
void buzzerError() {}
#else
static const int FREQ = 2000;
static const int SUCCESS_MS = 80;
static const int ERROR_MS = 80;
static const int PAUSE_MS = 100;

void buzzerInit() {
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
}

void buzzerSuccess() {
    tone(PIN_BUZZER, FREQ, SUCCESS_MS);
}

void buzzerError() {
    tone(PIN_BUZZER, FREQ, ERROR_MS);
    delay(ERROR_MS + PAUSE_MS);
    tone(PIN_BUZZER, FREQ, ERROR_MS);
}
#endif
