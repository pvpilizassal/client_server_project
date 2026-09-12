#include <QApplication>
#include "server_window.h"
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("TestTask"));
    QCoreApplication::setApplicationName(QStringLiteral("TimeServer"));

    ServerWindow w;
    w.show();
    return app.exec();
}
