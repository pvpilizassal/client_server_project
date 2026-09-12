#ifndef CLIENT_WINDOW_H
#define CLIENT_WINDOW_H

#include <QMainWindow>
#include <memory>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>

class ClientCore;
class ClientConfig;
class AnalogClock;

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTimerTick();
    void onTimeReceived(quint32 secondsFromMidnight);
    void onServerSettingsChanged();

private:
    void setupUi();
    void setupConnections();
    void setupTimer();

    void loadConfig();
    void saveConfig();
    void applyConfigToUi();

    void startCore();
    void restartCore();

    void updateServerStatusLabel(bool fresh);

    std::unique_ptr<ClientCore>   m_core;
    std::unique_ptr<ClientConfig> m_config;

    AnalogClock* m_clock          = nullptr;
    QLabel*      m_freshnessLabel = nullptr;
    QLineEdit*   m_addressEdit    = nullptr;
    QSpinBox*    m_portSpin       = nullptr;
    QTimer*      m_timer          = nullptr;

    bool m_dataFresh = false;
};

#endif // CLIENT_WINDOW_H
