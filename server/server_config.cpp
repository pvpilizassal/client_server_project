#include "server_config.h"
#include <QDebug>

// инициализация статических констант
const QString ServerConfig::PORT_KEY = "port";
const QString ServerConfig::TIME_KEY = "time";

ServerConfig::ServerConfig(const QString& filePath)
    : ConfigBase(filePath)
{
}

quint16 ServerConfig::getPort() const
{
    // если поле отсутствует или не int, вернется 0
    int value = m_json.value(PORT_KEY).toInt(0);
    return static_cast<quint16>(qBound(1, value, 65535)); // если 0, то 1
}

void ServerConfig::setPort(quint16 port)
{
    m_json[PORT_KEY] = static_cast<int>(port);
    // после изменения поля можно повторно вызвать validateAndFix для проверки
}

quint32 ServerConfig::getTimeSeconds() const
{
    int value = m_json.value(TIME_KEY).toInt(0);
    return static_cast<quint32>(qBound(0, value, 86399));
}

void ServerConfig::setTimeSeconds(quint32 seconds)
{
    m_json[TIME_KEY] = static_cast<int>(seconds);
}

QTime ServerConfig::getTime() const
{
    int secs = getTimeSeconds();
    return QTime(0, 0, 0).addSecs(secs);
}

void ServerConfig::setTime(const QTime& time)
{
    int secs = QTime(0, 0, 0).secsTo(time);
    setTimeSeconds(static_cast<quint32>(secs));
}

QJsonObject ServerConfig::getDefaults() const
{
    QJsonObject defaults;
    defaults[PORT_KEY] = 12345;
    defaults[TIME_KEY] = 0;   // 00:00
    return defaults;
}

void ServerConfig::validateAndFix()
{
    // проверка наличия и корректности порта
    if (m_json.contains(PORT_KEY)) {
        int port = m_json[PORT_KEY].toInt();
        if ((port < 1) || (port > 65535)) {
            qWarning() << "ServerConfig: port out of range, resetting to default";
            port = 12345;
        }
        m_json[PORT_KEY] = port;
    } else {
        qWarning() << "ServerConfig: missing 'port' key, setting default";
        m_json[PORT_KEY] = 12345;
    }

    // проверка наличия и корректности времени
    if (m_json.contains(TIME_KEY)) {
        int time = m_json[TIME_KEY].toInt();
        if ((time < 0) || (time > 86399)) {
            qWarning() << "ServerConfig: time out of range (0-86399), resetting to 0";
            time = 0;
        }
        m_json[TIME_KEY] = time;
    } else {
        qWarning() << "ServerConfig: missing 'time' key, setting default";
        m_json[TIME_KEY] = 0;
    }

    // тут можно проверить, что порт не занят
}

// хз использовать или нет
quint16 ServerConfig::fixPort(int value)
{
    if (value < 1) value = 1;
    if (value > 65535) value = 65535;
    return static_cast<quint16>(value);
}

quint32 ServerConfig::fixTime(int value)
{
    if (value < 0) value = 0;
    if (value > 86399) value = 86399;
    return static_cast<quint32>(value);
}
