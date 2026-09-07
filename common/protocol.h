#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>
#include <QTime>

namespace Protocol {
    const quint16 DEFAULT_PORT = 12345;
    const QString ALIVE_COMMAND = "ALIVE";
    const char TIME_SEPARATOR = ':';

    QString timeToString(const QTime& time);
    QTime stringToTime(const QString& str);
}

#endif // PROTOCOL_H