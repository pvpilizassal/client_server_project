#include "client_config.h"
#include <QDebug>
#include <QRegularExpression>

// инициализация статических констант
const QString ClientConfig::ADDRESS_KEY = "serverAddress";
const QString ClientConfig::PORT_KEY = "serverPort";

ClientConfig::ClientConfig(const QString& filePath)
    : ConfigBase(filePath)
{
}

QString ClientConfig::getServerAddress() const
{
    // если ключ отсутствует или строка пустая, возвращается "127.0.0.1" как fallback
    QString address = m_json.value(ADDRESS_KEY).toString();
    if (address.isEmpty()) {
        return "127.0.0.1";
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
        return 12345;
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

bool ClientConfig::isServerAddressValid() const
{
    return isValidAddress(getServerAddress());
}

QJsonObject ClientConfig::getDefaults() const
{
    QJsonObject defaults;
    defaults[ADDRESS_KEY] = "127.0.0.1";
    defaults[PORT_KEY] = 12345;
    return defaults;
}

void ClientConfig::validateAndFix()
{
    // проверка наличия и корректности порта
    if (m_json.contains(PORT_KEY)) {
        int port = m_json[PORT_KEY].toInt();
        if ((port < 1) || (port > 65535)) {
            qWarning() << "ClientConfig: serverPort out of range, resetting to 12345";
            port = 12345;
        }
        m_json[PORT_KEY] = port;
    } else {
        qWarning() << "ClientConfig: missing 'serverPort' key, setting default";
        m_json[PORT_KEY] = 12345;
    }

    // проверка наличия адреса
    if (!m_json.contains(ADDRESS_KEY)) {
        qWarning() << "ClientConfig: missing 'serverAddress' key, setting default";
        m_json[ADDRESS_KEY] = "127.0.0.1";
    } else {
        QString address = m_json[ADDRESS_KEY].toString();
        if (!isValidAddress(address)) {
            qWarning() << "ClientConfig: invalid serverAddress format, resetting to '127.0.0.1'";
            m_json[ADDRESS_KEY] = "127.0.0.1";
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

    // альтернативно можно разрешить доменные имена (проверка, что строка не пустая и состоит из допустимых символов)
    // для простоты - разрешена любую непустую строку без пробелов (домен или ip)
    // !!далее сделать более строгую проверку!!
    static QRegularExpression domainRegex(R"(^[a-zA-Z0-9.-]+$)");
    if (!address.isEmpty() && domainRegex.match(address).hasMatch()) {
        return true;
    }

    // если ничего не подошло – адрес невалидный
    return false;
}
