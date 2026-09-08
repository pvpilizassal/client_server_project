#include "server_core.h"
#include "../common/protocol.h"
#include <QDebug>
#include <QDataStream>

// отводим ресурсы под сокет, пока не устанавливаем его
ServerCore::ServerCore(QObject *parent)
    : QObject(parent)
    , m_socket(std::make_unique<QUdpSocket>())
{
}

// останавливается сервер (закрываем сокет)
ServerCore::~ServerCore()
{
    stop();
}

// запуск сервера на указанном порту
bool ServerCore::start(quint16 port)
{
    if (m_running) {
        qWarning() << "ServerCore already running";
        return false;
    }

    // сокет привязывается ко всем интерфейсам на указанном порту
    if (!m_socket->bind(QHostAddress::Any, port)) {
        qWarning() << "Failed to bind UDP socket on port" << port << ":" << m_socket->errorString();
        return false;
    }

    // подключаем сигнал readyRead к нашему слоту, который будет обрабаывать пакеты от клиента
    connect(m_socket.get(), &QUdpSocket::readyRead, this, &ServerCore::onReadyRead);

    m_running = true; // запустили сервер
    m_clientAlive = false; // клиент еще пока мертвый
    m_lastPacketTimer.start(); // таймер запустили, но пока клиент неизвестен

    // сигнал, что клиент пока мёртв
    emit clientAliveChanged(false);

    qDebug() << "ServerCore started on port" << port;
    return true;
}

// остановка сервера
void ServerCore::stop()
{
    if (m_running) {
        // закрываем сокет и возвращаем флаги в исходное состояние
        m_socket->close();
        m_running = false;
        m_clientAlive = false;
        emit clientAliveChanged(false); // отправляем сигнал, что клиент мертв
        qDebug() << "ServerCore stopped";
    }
}

// проверка, запущен ли сервер
bool ServerCore::isRunning() const
{
    return m_running;
}

// возвращает статус клиента (жив или мертв)
bool ServerCore::isClientAlive() const
{
    return m_clientAlive;
}

// отправка текущего времени клиенту
void ServerCore::sendTime(quint32 seconds)
{
}

// время в мс с последнего полученного пакета от клиента
// пока клиент не известен, надо возвращать число > 3 секунд, типа он мертв
qint64 ServerCore::timeSinceLastPacket() const
{
}


// обработчик входящих udp-пакетов
void ServerCore::onReadyRead()
{
}
