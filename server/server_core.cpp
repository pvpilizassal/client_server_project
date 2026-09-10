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

    // сокет привязывается к любому сетевому интерфейсу по конкретному порту
    // если порт занят - вернется фолз
    if (!m_socket->bind(QHostAddress::Any, port)) {
        qWarning() << "Failed to bind UDP socket on port" << port << ":" << m_socket->errorString();
        return false;
    }

    // подключаем сигнал readyRead к нашему слоту, который будет обрабаывать пакеты от клиента
    // основной поток не блокируется ожиданием данных
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
    // проверка, поднят ли сервер
    if (!m_running) {
        qWarning() << "ServerCore not running, cannot send time";
        return;
    }

    // проверка, известен ли клиент
    if (m_clientAddress.isNull() || m_clientPort == 0) {
        return;
    }

    // массив байтов для передачи времени в поток
    QByteArray datagramSeconds;
    // байтовый поток, который пишет из seconds в streamSeconds
    QDataStream streamSeconds(&datagramSeconds, QIODevice::WriteOnly);
    // явно указываем порядок записи BigEndian чтоб другие архитектуры правильно понимали
    streamSeconds.setByteOrder(QDataStream::BigEndian);
    streamSeconds << seconds;

    // отправляет upd-пакет (что отправляет, размер, адрес, порт)
    // тк в datagramSeconds пишется quint32 - размер датаграммы определен размером типа данных
    qint64 sent = m_socket->writeDatagram(datagramSeconds, m_clientAddress, m_clientPort);
    // обычно в случае неудачи возвращается -1 количество переданных байт
    if (sent == -1) {
        qWarning() << "Failed to send time to client:" << m_socket->errorString();
    } else {
        qDebug() << "Sent time" << seconds << "to" << m_clientAddress.toString() << ":" << m_clientPort;
    }
}

// время в мс с последнего полученного пакета от клиента
// пока клиент не известен, надо возвращать число > 3 секунд, типа он мертв
qint64 ServerCore::timeSinceLastPacket() const
{
    // если клиент мертв
    if (!m_clientAlive || m_clientAddress.isNull()) {
        return 999999; // большое число, чтобы условие > 3000 мс всегда срабатывало
    }

    // возвращает количество мс с последнего запуска QElapsedTimer
    return m_lastPacketTimer.elapsed();
}

// обработчик входящих udp-пакетов
void ServerCore::onReadyRead()
{
    // возвращает true, если есть хотя бы одна датаграмма, ожидающая обработки
    while (m_socket->hasPendingDatagrams()) {
        QByteArray buffer;
        // подгоняет буфер под размер полезных данных датаграммы (чтоб избежать потери данных), 0 отбрасывает
        buffer.resize(m_socket->pendingDatagramSize());
        QHostAddress senderAddress;
        quint16 senderPort;

        // buffer.data() - указатель на начало массива
        qint64 read = m_socket->readDatagram(buffer.data(), buffer.size(), &senderAddress, &senderPort);
        if (read == -1) {
            qWarning() << "Failed to read datagram:" << m_socket->errorString();
            continue;
        }

        buffer.resize(read);

        // проверка, что получен текст "ALIVE"
        QString msg = QString::fromUtf8(buffer).trimmed();
        if (msg == Protocol::ALIVE_COMMAND) {
            // обновляем адрес и порт клиента (если изменились)
            if (m_clientAddress != senderAddress || m_clientPort != senderPort) {
                m_clientAddress = senderAddress;
                m_clientPort = senderPort;
                qDebug() << "Client changed to" << senderAddress.toString() << ":" << senderPort;
            }

            // рестарт таймера
            m_lastPacketTimer.restart();

            // если клиент был мертв, меняем статус на живого
            if (!m_clientAlive) {
                m_clientAlive = true;
                emit clientAliveChanged(true);
                qDebug() << "Client became alive";
            }
        } else {
            qDebug() << "Received unknown packet from" << senderAddress.toString() << ":" << senderPort
                     << "size:" << read << ", content:" << msg;
        }
    }
}
