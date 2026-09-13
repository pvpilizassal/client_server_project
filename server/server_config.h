#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "../common/config_base.h"
#include "../common/protocol.h"
#include <QTime>

class ServerConfig : public ConfigBase
{
    static inline const QString PORT_KEY = QStringLiteral("port");
    static inline const QString TIME_KEY = QStringLiteral("time");

public:
    explicit ServerConfig(const QString& filePath);

    // геттеры и сеттеры
    quint16 getPort() const;
    void setPort(quint16 port);

    // возвращает путь к json-файлу настроек
    static QString defaultPath();

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
};

#endif // SERVER_CONFIG_H
