#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include "../common/config_base.h"
#include "../common/protocol.h"
#include <QHostAddress>

class ClientConfig : public ConfigBase
{
    static inline const QString ADDRESS_KEY = QStringLiteral("serverAddress");
    static inline const QString PORT_KEY = QStringLiteral("serverPort");

public:
    explicit ClientConfig(const QString& filePath);

    // геттеры и сеттеры
    QString getServerAddress() const;
    void setServerAddress(const QString& address);

    quint16 getServerPort() const;
    void setServerPort(quint16 port);

    static QString defaultPath();

protected:
    QJsonObject getDefaults() const override;
    void validateAndFix() override;

private:
    // вспомогательный метод для проверки строки адреса (может быть использован в validateAndFix)
    static bool isValidAddress(const QString& address);
};

#endif // CLIENT_CONFIG_H
