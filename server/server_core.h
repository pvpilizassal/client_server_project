#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include <QObject>

class ServerCore : public QObject
{
    Q_OBJECT
public:
    explicit ServerCore(QObject *parent = nullptr);

signals:
};

#endif // SERVER_CORE_H
