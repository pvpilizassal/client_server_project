#include "config_base.h"

ConfigBase::ConfigBase(const QString& filePath)
    : m_filePath(filePath)
{
}

bool ConfigBase::load()
{
    QFile file(m_filePath);

    // первый запуск, если файла не существовало - в настройки ставятся дефолты (разные)
    if (!file.exists()) {
        qDebug() << "Config file not found, using defaults. Path:" << m_filePath;
        setDefaults();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file for reading:" << m_filePath;
        setDefaults();
        return true;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON config file:" << parseError.errorString();
        // при повреждении файла устанавливаются дефолты, что делать со старым файлом?
        setDefaults();
        return true;
    }

    if (!doc.isObject()) {
        qWarning() << "Config file does not contain a JSON object.";
        setDefaults();
        return true;
    }

    m_json = doc.object();
    validateAndFix(); // валидация после загрузки
    return true;
}

bool ConfigBase::save() const // добавить сохранение при аварийном завершении
{
    QSaveFile file(m_filePath);
    // создается каталог, если его нет
    QDir dir = QFileInfo(file).absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create config directory:" << dir.absolutePath();
            return false;
        }
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << m_filePath;
        return false;
    }

    QJsonDocument doc(m_json);
    if (file.write(doc.toJson()) == -1) {
        qWarning() << "Failed to write config file:" << m_filePath;
        return false;
    }

    if (!file.commit()) {
        qWarning() << "Failed to commit config file (atomic rename failed)";
        return false;
    }

    return true;
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
