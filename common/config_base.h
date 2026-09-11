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

class ConfigBase
{
public:
    explicit ConfigBase(const QString& filePath); // пустой путь: = QString() мб стоит обработать ??
    virtual ~ConfigBase() = default;

    // запрет на копирование (просто ради дизайна)
    ConfigBase(const ConfigBase&) = delete;
    ConfigBase& operator=(const ConfigBase&) = delete;

    bool load();

    /**
     * @brief Сохраняет текущую конфигурацию в файл
     * @return true в случае успеха, иначе false
     */
    bool save() const;

    /**
     * @brief Устанавливает значения по умолчанию (вызывая getDefaults())
     */
    void setDefaults();

    /**
     * @brief Возвращает текущий JSON-объект конфигурации
     * Используется наследниками для доступа к параметрам
     */
    QJsonObject getJson() const;

    /**
     * @brief Защищённый метод для обновления всего JSON-объекта
     * Может быть использован наследниками при массовом изменении
     */
    void setJson(const QJsonObject& json);

    static QString defaultConfigPath(const QString& fileName);

protected:
    /**
     * @brief Чисто виртуальный метод, который должен вернуть объект
     * с дефолтными настройками для конкретного приложения
     * @return QJsonObject с полями и значениями по умолчанию
     */
    virtual QJsonObject getDefaults() const = 0;

    /**
     * @brief Валидация и корректировка значений после загрузки
     * По умолчанию ничего не делает. Наследники могут переопределить
     * для проверки типов, диапазонов и взаимосвязей полей
     */
    virtual void validateAndFix() = 0;

    QJsonObject m_json;      ///< Хранит все параметры конфигурации

private:
    QString m_filePath;      ///< Путь к файлу конфигурации
};

#endif // CONFIG_BASE_H
