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

#endif // CONFIG_H
