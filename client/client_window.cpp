#include "client_window.h"
#include "client_core.h"
#include "analogclock.h"
#include "client_config.h"
#include "../common/protocol.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFormLayout>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QMessageBox>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_core(std::make_unique<ClientCore>())
    , m_config(std::make_unique<ClientConfig>(ClientConfig::defaultPath()))
{
    setupUi();
    setupConnections();
    setupTimer();

    loadConfig();
    // applyConfigToUi();
    startCore();

    m_timer->start();

    setWindowTitle(tr("Клиент времени"));
    resize(300, 300);
}

ClientWindow::~ClientWindow()
{
    // saveConfig();
    m_core->stop();
}

void ClientWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void ClientWindow::loadConfig()
{
    const ConfigLoadResult res = m_config->load();

    // первый запуск
    if (res.status == ConfigLoadResult::Status::FileNotFound)
        return;

    if (!res.isWarning())
        return;

    const QString msg = res.message;
    QTimer::singleShot(0, this, [this, msg] {
        QMessageBox::warning(
            this,
            tr("Конфигурация"),
            tr("%1\nИспользованы значения по умолчанию.").arg(msg));
    });
}

void ClientWindow::saveConfig()
{
    const ConfigSaveResult res = m_config->save();

    if (res.isOk())
        return;

    qWarning() << "ClientWindow failed to save config:" << res.message;

    QMessageBox::warning(
        this,
        tr("Сохранение настроек"),
        tr("Не удалось сохранить настройки.\n\n%1\n\n"
           "Изменения будут потеряны при закрытии приложения.").arg(res.message));
}

void ClientWindow::setupUi()
{
    // auto* central = new QWidget(this);
    // setCentralWidget(central);

    // auto* mainLayout = new QVBoxLayout(central);
    // mainLayout->setContentsMargins(12, 12, 12, 12);
    // mainLayout->setSpacing(12);

    // m_clock = new AnalogClock(this);
    // mainLayout->addWidget(m_clock, 1);

    // m_freshnessLabel = new QLabel(this);
    // m_freshnessLabel->setAlignment(Qt::AlignCenter);
    // QFont statusFont = m_freshnessLabel->font();
    // statusFont.setPointSize(statusFont.pointSize() + 2);
    // m_freshnessLabel->setFont(statusFont);
    // mainLayout->addWidget(m_freshnessLabel);

    // updateServerStatusLabel(false);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    m_clock = new AnalogClock(this);
    mainLayout->addWidget(m_clock, 1);

    QFont statusFont = font();
    statusFont.setPointSize(statusFont.pointSize() + 2);

    auto* statusCaption = new QLabel(tr("Данные о времени актуальны - "), this);
    statusCaption->setFont(statusFont);
    statusCaption->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    m_freshnessLabel = new QLabel(this);
    m_freshnessLabel->setFont(statusFont);
    m_freshnessLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    auto* statusRow = new QHBoxLayout;
    statusRow->setContentsMargins(0, 0, 0, 0);
    statusRow->setSpacing(6);
    statusRow->addStretch();
    statusRow->addWidget(statusCaption);
    statusRow->addWidget(m_freshnessLabel);
    statusRow->addStretch();

    mainLayout->addLayout(statusRow);

    updateServerStatusLabel(false);
}

void ClientWindow::setupConnections()
{
    connect(m_core.get(), &ClientCore::timeReceived, this, &ClientWindow::onTimeReceived);
    connect(m_addressEdit, &QLineEdit::editingFinished, this, &ClientWindow::onServerSettingsChanged);
    connect(m_portSpin, &QSpinBox::editingFinished, this, &ClientWindow::onServerSettingsChanged);
}

void ClientWindow::setupTimer()
{
    m_timer = new QTimer(this);
    m_timer->setInterval(Protocol::TICK_INTERVAL_MS);
    connect(m_timer, &QTimer::timeout, this, &ClientWindow::onTimerTick);
}


void ClientWindow::applyConfigToUi()
{
    m_addressEdit->setText(m_config->getServerAddress());
    m_portSpin->setValue(m_config->getServerPort());
}

void ClientWindow::startCore()
{
    const QString addressString = m_config->getServerAddress();
    const QHostAddress address(addressString);

    if (address.isNull()) {
        qWarning() << "ClientWindow invalid server address:" << addressString;
        return;
    }

    if (!m_core->start(address, m_config->getServerPort())) {
        qWarning() << "ClientWindow failed to start ClientCore";
    }
}

void ClientWindow::restartCore()
{
    m_core->stop();

    m_dataFresh = false;
    updateServerStatusLabel(false);

    startCore();
}

void ClientWindow::onTimerTick()
{
    m_core->sendAlive();

    const bool freshNow = (m_core->timeSinceLastPacket() < Protocol::DATA_FRESH_TIMEOUT_MS);

    if (freshNow != m_dataFresh) {
        m_dataFresh = freshNow;
        updateServerStatusLabel(freshNow);
    }
}

void ClientWindow::onTimeReceived(quint32 secondsFromMidnight)
{
    const QTime time = QTime(0, 0).addSecs(static_cast<int>(secondsFromMidnight));
    m_clock->setTime(time);

    qDebug() << "ClientWindow time updated:" << time.toString(QStringLiteral("HH:mm"));
}

void ClientWindow::onServerSettingsChanged()
{
    const QString newAddress = m_addressEdit->text().trimmed();
    const quint16 newPort = static_cast<quint16>(m_portSpin->value());

    if (newAddress == m_config->getServerAddress() &&
        newPort == m_config->getServerPort()) {
        return;
    }

    m_config->setServerAddress(newAddress);
    m_config->setServerPort(newPort);

    qDebug() << "ClientWindow server settings changed to" << newAddress << ":" << newPort;

    restartCore();
    saveConfig();
}

void ClientWindow::updateServerStatusLabel(bool fresh)
{
    m_freshnessLabel->setText(fresh ? "Да" : "Нет");
}