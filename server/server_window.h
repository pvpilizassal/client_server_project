#ifndef SERVER_WINDOW_H
#define SERVER_WINDOW_H

#include <QMainWindow>
#include <QTime>
#include <memory>
#include <QLabel>
#include <QTimeEdit>
#include <QTimer>
#include <QApplication>

class ServerConfig;
class ServerCore;

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onTimerTick();
    void onTimeChanged(const QTime& newTime);

private:
    void setupUi();
    void setupConnections();
    void setupTimer();

    void loadConfig();
    void saveConfig();

    void updateTimeDisplay(const QTime& time);
    void updateClientStatusLabel(bool alive);

    std::unique_ptr<ServerCore>   m_core;
    std::unique_ptr<ServerConfig> m_config;

    QTimeEdit* m_timeEdit    = nullptr;
    QLabel*    m_statusLabel = nullptr;
    QTimer*    m_timer       = nullptr;

    QTime m_currentTime;
    bool  m_clientAlive = false;
};

#endif // SERVER_WINDOW_H
