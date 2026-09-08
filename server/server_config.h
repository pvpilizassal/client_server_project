#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "../common/config_base.h"
#include <QTime>

/**
 * @brief Конфигурация сервера
 *
 * Поля:
 * - port (quint16)    : UDP-порт для приёма соединений
 * - time (quint32)    : текущее время в секундах от полуночи [0..86399]
 *
 * Хранится в JSON-файле, например:
 * {
 *   "port": 12345,
 *   "time": 43200
 * }
 */
class ServerConfig : public ConfigBase
{
public:
    explicit ServerConfig(const QString& filePath);

    // геттеры и сеттеры
    quint16 getPort() const;
    void setPort(quint16 port);

    quint32 getTimeSeconds() const;
    void setTimeSeconds(quint32 seconds);

    // методы для работы с QTime
    QTime getTime() const;
    void setTime(const QTime& time);

protected:
    // реализация чисто виртуального метода
    QJsonObject getDefaults() const override;

    // переопределение метода валидации
    void validateAndFix() override;

private:
    // ключи для доступа к JSON-полям
    static const QString PORT_KEY;
    static const QString TIME_KEY;

    // ?метод для проверки и коррекции порта?
    static quint16 fixPort(int value);

    // ?метод для проверки и коррекции времени?
    static quint32 fixTime(int value);
};

#endif // SERVER_CONFIG_H
