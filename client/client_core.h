#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#include <QObject>

class ClientCore : public QObject
{
    Q_OBJECT
public:
    explicit ClientCore(QObject *parent = nullptr);

signals:
};

#endif // CLIENT_CORE_H
