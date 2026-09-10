#include <QApplication>
#include "client_window.h"
#include "client_config.h"
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName(QStringLiteral("TestTask"));
    QCoreApplication::setApplicationName(QStringLiteral("TimeClient"));

    QMainWindow w;
    w.show();
    return app.exec();
}
