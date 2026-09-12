#include "server_window.h"
#include "server_core.h"
#include "../common/protocol.h"
#include "server_config.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimeEdit>
#include <QTimer>
#include <QVBoxLayout>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent) // вызов конструктора базового класса
    , m_core(std::make_unique<ServerCore>()) // объект ServerCore без родителя чтоб не было двойного удаления
    , m_config(std::make_unique<ServerConfig>(ServerConfig::defaultPath())) // defaultPath() - не родитель,
                                    // а параметр для конструктора ServerConfig, который ожидает путь к файлу
{
    setupUi(); // виджеты, компоновка
    setupConnections(); // подключение сигналов к слотам
    setupTimer(); // QTimer с интервалом из Protocol

    loadConfig(); // читаем json, заполняем поля m_config

    const quint16 port = m_config->getPort(); // вытаскиваем из json порт
    if (!m_core->start(port)) { // подключаем сокет к порту на любом интерфейсе
        QMessageBox::critical(
            this,
            tr("Ошибка запуска"),
            tr("Не удалось запустить сервер на порту %1.\n"
               "Возможно, порт уже занят другим приложением.").arg(port));
    }

    m_currentTime = m_config->getTime(); // читаем последнее время из json
    updateTimeDisplay(m_currentTime);

    m_timer->start(); // запуск таймера

    setWindowTitle(tr("Сервер времени"));

    resize(300, 100);
}

ServerWindow::~ServerWindow()
{
    // saveConfig();
    m_core->stop();
}

void ServerWindow::closeEvent(QCloseEvent *event)
{
    m_config->setTime(m_currentTime);
    const ConfigSaveResult res = m_config->save();

    if (res.isOk()) {
        event->accept();
        return;
    }

    // если не получилось сохранить файл настроек — говорим пользователю и даём выбор
    const auto answer = QMessageBox::warning(
        this,
        tr("Сохранение настроек"),
        tr("Не удалось сохранить настройки при закрытии.\n\n%1\n\n"
           "Закрыть приложение без сохранения?").arg(res.message),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);   // дефолт — No

    if (answer == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();    // окно остаётся открытым
}

bool ServerWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == centralWidget() && event->type() == QEvent::MouseButtonPress) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);

        QWidget* clicked = QApplication::widgetAt(mouseEvent->globalPosition().toPoint());

        const bool clickedInsideTimeEdit =
            clicked == m_timeEdit || m_timeEdit->isAncestorOf(clicked);

        if (!clickedInsideTimeEdit && m_timeEdit->hasFocus()) {
            m_timeEdit->clearFocus();
        }
    }

    if (watched == m_timeEdit && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            m_timeEdit->clearFocus();
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void ServerWindow::setupUi()
{
    auto* central = new QWidget(this);
    central->setFocusPolicy(Qt::StrongFocus);
    setCentralWidget(central);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(6, 1, 6, 1);
    mainLayout->setSpacing(2);

    QFont statusFont = font();
    statusFont.setPointSize(statusFont.pointSize() + 2);

    m_timeEdit = new QTimeEdit(this);
    m_timeEdit->setDisplayFormat(QStringLiteral("HH:mm"));
    m_timeEdit->setWrapping(true); // цикличность времени

    // сигнал timeChanged приходит только по завершении ввода
    // (Enter, потеря фокуса, клик по стрелкам), а не на каждый символ
    m_timeEdit->setKeyboardTracking(false);
    m_timeEdit->setAlignment(Qt::AlignCenter);
    m_timeEdit->setMinimumWidth(150);
    m_timeEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_timeEdit->setFont(statusFont);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    auto* statusRow = new QHBoxLayout;
    auto* statusCaption = new QLabel(tr("Клиент видит время - "), this);

    // статус клиента
    statusRow->setContentsMargins(0, 0, 0, 0);
    statusRow->addStretch();
    statusCaption->setFont(statusFont);
    m_statusLabel->setFont(statusFont);
    statusRow->addWidget(statusCaption);
    statusRow->addWidget(m_statusLabel);
    statusRow->addStretch();

    // время
    mainLayout->addStretch();
    mainLayout->addWidget(m_timeEdit, 0, Qt::AlignHCenter);
    mainLayout->addLayout(statusRow);
    mainLayout->addStretch();

    // начальное состояние — Нет, пока не пришёл первый пакет
    updateClientStatusLabel(false);

    // окно подгоняется под содержимое
    adjustSize();
    setMinimumSize(sizeHint());
    // снять фокус кликом по полю окна
    centralWidget()->installEventFilter(this);
    // снять фокус нажатием на enter
    m_timeEdit->installEventFilter(this);
}

void ServerWindow::setupConnections()
{
    connect(m_timeEdit, &QTimeEdit::timeChanged, this, &ServerWindow::onTimeChanged);
}

void ServerWindow::setupTimer()
{
    m_timer = new QTimer(this);
    m_timer->setInterval(Protocol::TICK_INTERVAL_MS); // минимальный интервал
    connect(m_timer, &QTimer::timeout, this, &ServerWindow::onTimerTick);
}

void ServerWindow::loadConfig()
{
    const ConfigLoadResult res = m_config->load();

    // первый запуск
    if (res.status == ConfigLoadResult::Status::FileNotFound)
        return;

    if (!res.isWarning())
        return;

    // откладываем показ до момента, когда окно уже будет показано
    const QString msg = res.message;
    QTimer::singleShot(0, this, [this, msg] {
        QMessageBox::warning(
            this,
            tr("Конфигурация"),
            tr("%1\nИспользованы значения по умолчанию.").arg(msg));
    });
}

// void ServerWindow::saveConfig()
// {
//     // сохраняем последнее время в конфиг
//     m_config->setTime(m_currentTime);
//     if (!m_config->save())
//         qWarning() << "ServerWindow failed to save config";
// }

void ServerWindow::onTimerTick()
{
    // автоинкремент - выполняется всегда независимо от того, редактирует ли пользователь поле
    m_currentTime = m_currentTime.addSecs(Protocol::TIME_INCREMENT_SECONDS);

    // обновляем поле только если пользователь его не редактирует,
    //    чтобы не перезатирать ввод
    if (!m_timeEdit->hasFocus()) {
        updateTimeDisplay(m_currentTime);
    }

    // отправляем время Клиенту (считая секунды от 00:00:00 до m_currentTime)
    const quint32 secondsFromMidnight = static_cast<quint32>(QTime(0, 0, 0).secsTo(m_currentTime));
    m_core->sendTime(secondsFromMidnight);

    // проверяем актуальность клиента по таймауту
    const bool aliveNow = (m_core->timeSinceLastPacket() < Protocol::CLIENT_ALIVE_TIMEOUT_MS);

    if (aliveNow != m_clientAlive) {
        m_clientAlive = aliveNow;
        updateClientStatusLabel(aliveNow);
    }

    qDebug() << "ServerWindow tick time ="
             << m_currentTime.toString(QStringLiteral("HH:mm"))
             << " client alive =" << aliveNow;
}

void ServerWindow::onTimeChanged(const QTime& newTime)
{
    // программные изменения заблокированы в updateTimeDisplay(),
    // поэтому сюда попадаем только при действии пользователя
    if (!newTime.isValid()) {
        return;
    }
    m_currentTime = newTime;
    qDebug() << "ServerWindow user set time to" << newTime.toString(QStringLiteral("HH:mm"));
}

void ServerWindow::updateTimeDisplay(const QTime& time)
{
    m_timeEdit->blockSignals(true);
    m_timeEdit->setTime(time);
    m_timeEdit->blockSignals(false);
}

void ServerWindow::updateClientStatusLabel(bool alive)
{
    if (alive) m_statusLabel->setText(tr("Да"));
    else m_statusLabel->setText(tr("Нет"));
}
