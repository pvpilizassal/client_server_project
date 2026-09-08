#include "protocol.h"

namespace Protocol {

QString timeToString(const QTime& time) {
    return time.toString("HH:mm");
}

QTime stringToTime(const QString& str) {
    return QTime::fromString(str, "HH:mm");
}

}
