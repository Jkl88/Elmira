/**
 * power_save.cpp — учёт активности, задержка при простое, понижение CPU, отключение BT
 * Не используем light sleep (WiFi бы отключился — GET/веб перестали бы работать).
 */

#include "power_save.h"
#include "config.h"

#if !POWER_SAVE_ENABLE

void powerSaveReportActivity() {}
void powerSaveSetup() {}
void powerSaveTick() {}

#else

#include <esp32-hal-cpu.h>

#ifdef ESP32
#include "esp_bt.h"
#endif

static uint32_t _lastActivityMs = 0;
static bool _cpuReduced = false;

void powerSaveReportActivity() {
    _lastActivityMs = millis();
    if (_cpuReduced) {
        setCpuFrequencyMhz(160);
        _cpuReduced = false;
    }
}

void powerSaveSetup() {
    _lastActivityMs = millis();
#ifdef ESP32
    // Полное отключение Bluetooth — экономия ~30–80 мА (ESP32-C3: BLE не используется)
    (void)esp_bt_controller_disable();
#endif
    // В STA режиме modem sleep уже по умолчанию (WiFi просыпается по DTIM и при приёме).
}

void powerSaveTick() {
    uint32_t now = millis();
    uint32_t idle = now - _lastActivityMs;

    if (idle >= (uint32_t)POWER_SAVE_IDLE_CPU_MS && !_cpuReduced) {
        setCpuFrequencyMhz(80);
        _cpuReduced = true;
    }

    if (idle >= (uint32_t)POWER_SAVE_IDLE_DELAY_MS) {
        delay(POWER_SAVE_IDLE_LOOP_MS);
    }
}

#endif // POWER_SAVE_ENABLE
