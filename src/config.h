/**
 * config.h — конфигурация пинов и параметров замка Elmira
 * Все параметры задаются через #define для лёгкой подстройки под железо
 */

#ifndef CONFIG_H
#define CONFIG_H

// ─── Пароль по умолчанию (8 цифр), если в NVS ещё ничего нет ───
#define LOCK_DEFAULT_PASSWORD     "12341234"
#define LOCK_PASSWORD_LEN         8

// ─── Пины (ESP32-C3 SuperMini — подставить свои при необходимости) ───
#define PIN_SERVO                 6   // ШИМ сервы 360°
#define PIN_HALL_OPEN              7   // Датчик холла «открыто»
#define PIN_HALL_CLOSED            8   // Датчик холла «закрыто»
// AS5600 по I2C (Wire): SDA=4, SCL=5 — типично для C3
#define PIN_SDA                   4
#define PIN_SCL                   5
// Пищалка для звуковой обратной связи (0 = отключена)
#define PIN_BUZZER                3

// ─── Направление вращения сервы ───
// Для открытия и закрытия серва крутится в одну сторону до срабатывания датчика.
// Значение: 1 = в одну сторону, -1 = в другую (зависит от подключения).
#define SERVO_DIR_OPEN            1
#define SERVO_DIR_CLOSE          -1

// ─── Таймаут сервы (мс): если датчик не сработал — останавливаем и ошибка ───
#define SERVO_TIMEOUT_MS          30000

// ─── Скорость «аварийного закрытия»: пауза между шагами (мс), чем больше — тем медленнее ───
#define SERVO_EMERGENCY_STEP_MS   50

// ─── AS5600: углы в градусах (0..360) для магнитного ключа ───
// Начальный угол «правильной» ориентации магнита; поворот на 90° по часовой = открытие.
#define AS5600_ANGLE_START_DEG    0
#define AS5600_ANGLE_END_DEG      90

// ─── NVS namespace и ключ для хранения пароля ───
#define NVS_NAMESPACE             "elmira"
#define NVS_KEY_PASSWORD         "pass"
#define NVS_KEY_AS5600_START      "as5600_start"
#define NVS_KEY_AS5600_END        "as5600_end"
#define NVS_KEY_HALL_OPEN_OPEN    "ho_open"   // значение пина «открыто» в положении открыто (0/1)
#define NVS_KEY_HALL_OPEN_CLOSED  "ho_closed" // значение пина «открыто» в положении закрыто
#define NVS_KEY_HALL_CLOSED_OPEN  "hc_open"   // значение пина «закрыто» в положении открыто
#define NVS_KEY_HALL_CLOSED_CLOSED "hc_closed"

// ─── Энергосбережение (батарея) ───
// После этого времени без активности: добавляем задержку в loop (мс)
#define POWER_SAVE_IDLE_DELAY_MS  10000
// После этого времени: понижаем частоту CPU до 80 МГц (мс)
#define POWER_SAVE_IDLE_CPU_MS    60000
// Задержка в loop при простое (мс), чтобы не крутить CPU впустую
#define POWER_SAVE_IDLE_LOOP_MS   50
// 1 = включить экономию (CPU 80 МГц, задержка, BT выкл), 0 = выключить
#define POWER_SAVE_ENABLE         1

#endif // CONFIG_H
