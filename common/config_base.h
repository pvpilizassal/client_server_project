#ifndef CONFIG_BASE_H
#define CONFIG_BASE_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QSaveFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

struct ConfigLoadResult
{
    enum class Status {
        Ok,                  // файл прочитан и распарсен
        FileNotFound,        // файла нет — первый запуск, применены дефолты
        OpenFailed,          // файл есть, но не открылся (права и т.п.)
        ParseError,          // битый JSON
        NotAnObject,         // валидный JSON, но не объект
        ValidationFixed      // загружен, но валидация что-то поправила
    };

    Status  status = Status::Ok;
    QString message;         // описание для UI

    bool isOk()      const { return status == Status::Ok; }
    bool isWarning() const { return status != Status::Ok; }
};

struct ConfigSaveResult
{
    enum class Status {
        Ok,
        DirCreateFailed,   // не удалось создать каталог
        OpenFailed,        // файл не открылся (права, read-only, антивирус)
        WriteFailed,       // ошибка записи (диск переполнен, обрыв)
        CommitFailed       // атомарный rename не прошёл
    };

    Status  status = Status::Ok;
    QString message;       // описание для UI

    bool isOk()      const { return status == Status::Ok; }
    bool isWarning() const { return status != Status::Ok; }
};

class ConfigBase
{
public:
    explicit ConfigBase(const QString& filePath); // пустой путь: = QString() мб стоит обработать ??
    virtual ~ConfigBase() = default;

    // запрет на копирование (просто ради дизайна)
    ConfigBase(const ConfigBase&) = delete;
    ConfigBase& operator=(const ConfigBase&) = delete;

    ConfigLoadResult load();
    ConfigSaveResult save() const;
    void setDefaults();

    QJsonObject getJson() const;
    void setJson(const QJsonObject& json);

    static QString defaultConfigPath(const QString& fileName);

protected:
    virtual QJsonObject getDefaults() const = 0;
    virtual void validateAndFix() = 0;

    QJsonObject m_json; // хранит параметры конфигурации

private:
    QString m_filePath;
};

#endif // CONFIG_BASE_H
