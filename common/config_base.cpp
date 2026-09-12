#include "config_base.h"

ConfigBase::ConfigBase(const QString& filePath)
    : m_filePath(filePath)
{
}

ConfigLoadResult ConfigBase::load()
{
    QFile file(m_filePath);

    // первый запуск, если файла не существовало - в настройки ставятся дефолты (разные)
    if (!file.exists()) {
        qDebug() << "Config file not found, using defaults. Path:" << m_filePath;
        setDefaults();
        return { ConfigLoadResult::Status::FileNotFound,
                 QString("Файл конфигурации не найден.") };
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file for reading:" << m_filePath;
        setDefaults();
        return { ConfigLoadResult::Status::OpenFailed,
                 QString("Не удалось открыть файл конфигурации: %1").arg(file.errorString()) };
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON config file:" << parseError.errorString();
        // при повреждении файла устанавливаются дефолты, что делать со старым файлом?
        setDefaults();
        return { ConfigLoadResult::Status::ParseError,
                 QString("Файл конфигурации повреждён (JSON-ошибка на позиции %1): %2")
                    .arg(parseError.offset).arg(parseError.errorString()) };
    }

    if (!doc.isObject()) {
        qWarning() << "Config file does not contain a JSON object.";
        setDefaults();
        return { ConfigLoadResult::Status::NotAnObject,
                QString("Файл конфигурации не содержит JSON-объект.") };
    }

    m_json = doc.object();
    validateAndFix(); // валидация после загрузки
    return { ConfigLoadResult::Status::Ok, {} };
}

ConfigSaveResult ConfigBase::save() const // добавить сохранение при аварийном завершении
{
    QSaveFile file(m_filePath);
    // создается каталог, если его нет
    const QDir dir = QFileInfo(file).absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create config directory:" << dir.absolutePath();
        return { ConfigSaveResult::Status::DirCreateFailed,
                QString("Не удалось создать каталог: %1").arg(dir.absolutePath()) };
    }

    // создает временный файл
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << m_filePath << file.errorString();
        return { ConfigSaveResult::Status::OpenFailed,
                QString("Не удалось открыть файл для записи: %1").arg(file.errorString()) };
    }

    // пишет во временный файл
    QJsonDocument doc(m_json);
    if (file.write(doc.toJson()) == -1) {
        qWarning() << "Failed to write config file:" << m_filePath << file.errorString();
        return { ConfigSaveResult::Status::WriteFailed,
                QString("Не удалось записать файл: %1").arg(file.errorString()) };
    }

    // атомарно переименовывает временный файл в рабочий, иначе удаляет временный
    if (!file.commit()) {
        qWarning() << "Failed to commit config file (atomic rename failed)" << file.errorString();
        return { ConfigSaveResult::Status::CommitFailed,
                QString("Не удалось атомарно заменить файл: %1").arg(file.errorString()) };
    }

    return {};
}

void ConfigBase::setDefaults()
{
    m_json = getDefaults();
    validateAndFix(); // также валидируем дефолты (на случай, если наследник ошибся)
}

QJsonObject ConfigBase::getJson() const
{
    return m_json;
}

void ConfigBase::setJson(const QJsonObject& json)
{
    m_json = json;
    validateAndFix();
}
