/**
 * lock_password.h — хранение и проверка пароля замка в NVS (8 цифр)
 */

#ifndef LOCK_PASSWORD_H
#define LOCK_PASSWORD_H

#include <Arduino.h>

/// Инициализация NVS и загрузка пароля (или установка дефолтного)
void lockPasswordInit();

/// Проверка пароля (строка 8 цифр). Возвращает true при совпадении.
bool lockPasswordCheck(const String& input);

/// Установка нового пароля (только 8 цифр), сохраняется в NVS
bool lockPasswordSet(const String& newPass);

/// Текущий пароль (для отладки не выводить в лог). 8 символов.
String lockPasswordGet();

#endif // LOCK_PASSWORD_H
