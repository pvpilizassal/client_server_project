#include "client_core.h"
#include "../common/protocol.h"

#include <QDebug>
#include <QDataStream>

ClientCore::ClientCore(QObject *parent)
    : QObject(parent)
    , m_socket(std::make_unique<QUdpSocket>())
{
    connect(m_socket.get(), &QUdpSocket::readyRead,
            this, &ClientCore::onReadyRead);
}

ClientCore::~ClientCore()
{
    stop();
}

bool ClientCore::start(const QHostAddress& serverAddress, quint16 serverPort)
{
    if (m_running) {
        qWarning() << "ClientCore already running";
        return false;
    }

    if (serverAddress.isNull() || serverPort == 0) {
        qWarning() << "ClientCore: invalid server address or port";
        return false;
    }

    m_serverAddress = serverAddress;
    m_serverPort = serverPort;


    m_running = true;
    m_dataFresh = false;
    m_lastPacketTimer.start(); // «нулевой» отсчёт до первого пакета

    emit dataFreshnessChanged(false);

    qDebug() << "ClientCore started, target =" << serverAddress.toString()
             << ":" << serverPort;
    return true;
}

void ClientCore::stop()
{
    if (!m_running) {
        return;
    }

    m_socket->close();
    m_running = false;
    m_dataFresh = false;

    emit dataFreshnessChanged(false);

    qDebug() << "ClientCore stopped";
}

bool ClientCore::isRunning() const
{
    return m_running;
}

void ClientCore::sendAlive()
{
    if (!m_running) {
        qWarning() << "ClientCore not running, cannot send ALIVE";
        return;
    }

    const QByteArray datagram = Protocol::ALIVE_COMMAND.toUtf8();
    const qint64 sent = m_socket->writeDatagram(datagram,
                                                m_serverAddress,
                                                m_serverPort);
    if (sent == -1) {
        qWarning() << "Failed to send ALIVE:" << m_socket->errorString();
    }
}


qint64 ClientCore::timeSinceLastPacket() const
{

    if (!m_dataFresh) {
        return std::numeric_limits<qint64>::max();
    }
    return m_lastPacketTimer.elapsed();
}

bool ClientCore::isDataFresh() const
{
    return m_dataFresh;
}

void ClientCore::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(m_socket->pendingDatagramSize());

        QHostAddress senderAddress;
        quint16 senderPort = 0;

        const qint64 read = m_socket->readDatagram(buffer.data(),
                     buffer.size(), &senderAddress, &senderPort);
        if (read == -1) {
            qWarning() << "Failed to read datagram:" << m_socket->errorString();
            continue;
        }
        buffer.resize(static_cast<int>(read));


        if (senderPort != m_serverPort) {
            qDebug() << "Ignoring datagram from unexpected port"
                     << senderPort;
            continue;
        }

        // ожидаем ровно 4 байта (quint32 BigEndian).
        if (buffer.size() != static_cast<int>(sizeof(quint32))) {
            qDebug() << "Ignoring datagram of unexpected size"
                     << buffer.size();
            continue;
        }

        quint32 seconds = 0;
        QDataStream stream(&buffer, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream >> seconds;

        if (seconds >= 24 * 60 * 60) {
            qWarning() << "Received out-of-range time value:" << seconds;
            continue;
        }

        m_lastPacketTimer.restart();

        if (!m_dataFresh) {
            m_dataFresh = true;
            emit dataFreshnessChanged(true);
        }

        emit timeReceived(seconds);

        qDebug() << "Received time:" << seconds << "s from"
                 << senderAddress.toString() << ":" << senderPort;
    }
}
