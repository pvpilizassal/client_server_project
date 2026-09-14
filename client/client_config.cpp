#include "client_config.h"
#include <QDebug>
#include <QRegularExpression>

ClientConfig::ClientConfig(const QString& filePath)
    : ConfigBase(filePath)
{
}

QString ClientConfig::getServerAddress() const
{
    // если ключ отсутствует или строка пустая, возвращается "127.0.0.1" как fallback
    QString address = m_json.value(ADDRESS_KEY).toString();
    if (address.isEmpty()) {
        return Protocol::DEFAULT_ADDRESS;
    }
    return address;
}

void ClientConfig::setServerAddress(const QString& address)
{
    m_json[ADDRESS_KEY] = address;
}

quint16 ClientConfig::getServerPort() const
{
    int value = m_json.value(PORT_KEY).toInt(0);
    // приведение к допустимому диапазону 1..65535, если значение некорректно – возвращается 12345
    if ((value < 1) || (value > 65535)) {
        return Protocol::DEFAULT_PORT;
    }
    return static_cast<quint16>(value);
}

void ClientConfig::setServerPort(quint16 port)
{
    m_json[PORT_KEY] = static_cast<int>(port);
}

QString ClientConfig::defaultPath()
{
    // Linux ~/.config/<OrgName>/<AppName>/
    // Windows	C:/Users/<User>/AppData/Local/<OrgName>/<AppName>/
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return dir + QStringLiteral("/client_config.json");
}

QJsonObject ClientConfig::getDefaults() const
{
    QJsonObject defaults;
    defaults[ADDRESS_KEY] = Protocol::DEFAULT_ADDRESS;
    defaults[PORT_KEY] = Protocol::DEFAULT_PORT;
    return defaults;
}

void ClientConfig::validateAndFix()
{
    // проверка наличия и корректности порта
    if (m_json.contains(PORT_KEY)) {
        int port = m_json[PORT_KEY].toInt();
        if ((port < 1) || (port > 65535)) {
            qWarning() << "ClientConfig: serverPort out of range, resetting to 12345";
            port = Protocol::DEFAULT_PORT;
        }
        m_json[PORT_KEY] = port;
    } else {
        qWarning() << "ClientConfig: missing 'serverPort' key, setting default";
        m_json[PORT_KEY] = Protocol::DEFAULT_PORT;
    }

    // проверка наличия адреса
    if (!m_json.contains(ADDRESS_KEY)) {
        qWarning() << "ClientConfig: missing 'serverAddress' key, setting default '127.0.0.1'";
        m_json[ADDRESS_KEY] = Protocol::DEFAULT_ADDRESS;
    } else {
        QString address = m_json[ADDRESS_KEY].toString();
        if (!isValidAddress(address)) {
            qWarning() << "ClientConfig: invalid serverAddress format, resetting to '127.0.0.1'";
            m_json[ADDRESS_KEY] = Protocol::DEFAULT_ADDRESS;
        }
    }
}

bool ClientConfig::isValidAddress(const QString& address)
{
    // проверка является ли строка валидным ipv4-адресом
    QHostAddress host;
    if (host.setAddress(address) && host.protocol() == QAbstractSocket::IPv4Protocol) {
        return true;
    }

    return false;
}
