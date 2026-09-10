#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>
#include <QTime>
#include <QtGlobal>

// inline (для переменных, начиная с C++17) - правило компоновки. при подключении пространства имен
// в несколько файлов, каждый файл не получит свою копию переменных, сущность останется единой
// constexpr - подстановка на этапе компиляции
namespace Protocol {
    // порт по умолчанию
    inline constexpr quint16 DEFAULT_PORT = 12345;
    // команда "жизни" от Клиента
    inline const QString ALIVE_COMMAND = QStringLiteral("ALIVE");
    // интервал инкремента таймера 1 Гц
    inline constexpr int TICK_INTERVAL_MS = 1000;
    // шаг времени за 1 тик (60 секунд = 1 минута за 1 тик)
    inline constexpr int TIME_INCREMENT_SECONDS = 60;
    // интервал ожидания пакета жизни от Клиента
    inline constexpr qint64 CLIENT_ALIVE_TIMEOUT_MS = 3000;
    // интервал ожидания данных о времени от Сервера
    inline constexpr qint64 DATA_FRESH_TIMEOUT_MS = 3000;
}

#endif // PROTOCOL_H
