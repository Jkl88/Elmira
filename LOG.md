# Лог прогресса — Elmira

## 2026-03-06

### Создание проекта (PlatformIO, Arduino)

- **platformio.ini** — env `esp32-c3-devkitm-1`, framework arduino, LittleFS, lib_deps: TKWifiManager (GitHub), WebSockets, Adafruit AS5600 (GitHub), Adafruit BusIO, ArduinoJson, Servo (arduino-libraries).
- **src/config.h** — пины (серва 6, холлы 7/8, I2C 4/5), таймаут сервы 30 с, углы AS5600 0°/90°, пароль по умолчанию 12341234, NVS namespace/ключ.
- **src/hardware/hall_sensors.*** — инициализация и опрос двух цифровых датчиков холла; `hallGetState()` → 0 (закрыто), 1 (открыто), -1 (неизвестно).
- **src/hardware/servo_driver.*** — серва 360°: `servoRun(direction)`, `servoRunSlow(direction)` для аварийного закрытия, `servoStop()`, таймаут.
- **src/hardware/as5600_driver.*** — AS5600 по I2C, `as5600GetAngle()` (градусы), `as5600CheckKeyOpened()` (стартовый угол + поворот 90°), `as5600ResetKeyState()`.
- **src/lock_state.*** — машина состояний: closed / open / in_progress / unknown; открытие/закрытие по запросу с проверкой холлов; аварийное закрытие (медленная серва до датчика «закрыто»); при неопределённости — останов сервы, состояние «неизвестно».
- **src/lock_password.*** — NVS, загрузка/сохранение пароля (8 цифр), проверка, дефолт 12341234.
- **src/lock_web.*** — регистрация в TKWifiManager: GET `/open?pass=...`, GET `/close`; WebSocket: команды `getState`, `close`, `emergencyClose`, `{"cmd":"open","pass":"..."}`, `{"cmd":"setPassword","pass":"..."}` (смена только при открытом замке); рассылка состояния `{"type":"lock","state":"..."}`.
- **src/main.cpp** — setup: lockPasswordInit, lockStateInit, as5600Init, wifiMgr.begin("Elmira"), lockWebSetup; loop: wifiMgr.loop(), lockStateTick(), проверка магнитного ключа (as5600CheckKeyOpened → lockRequestOpen), рассылка состояния при изменении.
- **data/index.html** — главная страница: 8 полей пароля (автофокус), кнопка ОТКРЫТЬ/ЗАКРЫТЬ, «Сменить пароль», «Аварийное закрытие»; индикация состояния (зелёный/красный/оранжевый/неизвестно); модалки для ошибок и смены пароля; WebSocket на порт 81; стиль как в TKWifiManager (CSS-переменные).
- **data/theme.css**, **data/theme.js** — темы и переключатель в стиле TKWifiManager.
- **README.md** — описание проекта, сборка, конфигурация, структура.
- **LOG.md** — этот файл.

### Сборка

- Первая сборка: ошибки по Servo (подключена arduino-libraries/Servo) и по API Adafruit AS5600 (getAngle вместо readAngle, убран setDirection). Исправлено.
- Итог: **сборка успешна** (RAM ~13%, Flash ~47%).

### Что осталось на доработку

- Реальное тестирование на железе (подключение пинов, калибровка AS5600, направление сервы).
- При питании от батареи — энергосбережение (light sleep / понижение частоты при простое).
