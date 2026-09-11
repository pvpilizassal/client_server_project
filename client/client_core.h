#ifndef CLIENT_CORE_H
#define CLIENT_CORE_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QElapsedTimer>
#include <memory>

class ClientCore : public QObject
{
    Q_OBJECT
public:
    explicit ClientCore(QObject *parent = nullptr);
    ~ClientCore() override;

    bool start(const QHostAddress& serverAddress, quint16 serverPort);

    void stop();
    bool isRunning() const;


    void sendAlive();
    qint64 timeSinceLastPacket() const;

    bool isDataFresh() const;

signals:

    void timeReceived(quint32 secondsFromMidnight);

    void dataFreshnessChanged(bool fresh);

private slots:
    void onReadyRead();

private:
    std::unique_ptr<QUdpSocket> m_socket;
    QHostAddress m_serverAddress;
    quint16      m_serverPort       = 0;
    QElapsedTimer m_lastPacketTimer;
    bool         m_dataFresh        = false;
    bool         m_running          = false;
};

#endif // CLIENT_CORE_H
