#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QElapsedTimer>
#include <memory>

// таймер на 3 секунды для проверки жизни клиента
// метод обработки таймаута

class ServerCore : public QObject
{
    Q_OBJECT
public:
    ServerCore(QObject *parent = nullptr); // в конструкторе отводим ресурс под сокет
    ~ServerCore(); // в деструкторе отвобождается сокет и флаги ставятся в фолз

    bool start(quint16 port); // запускаем сервер
    void stop(); // останавливаем сервер
    bool isRunning() const; // проверка, запущен ли вервер

    void sendTime(quint32 seconds); // отправка времени с сервера клиенту
    qint64 timeSinceLastPacket() const; // сколько времени прошло с последнего пинга от клиента (должно быть < 3 сек)
    bool isClientAlive() const; // жив ли клиент

signals:
    void clientAliveChanged(bool alive); // сигнал о статусе жизни клиента

private slots:
    void onReadyRead(); // обработка пакета от клиента

private:
    std::unique_ptr<QUdpSocket> m_socket; // сокет
    QHostAddress m_clientAddress; // айпи адрес клиента
    quint16 m_clientPort = 0; // порт клиента
    QElapsedTimer m_lastPacketTimer; // таймер с последнего пинга от клиента
    bool m_clientAlive = false; // статус жизни клиента
    bool m_running = false; // статус, поднят сервер или нет

};

#endif // SERVER_CORE_H
