#include <QApplication>
#include "server_window.h"
#include "server_config.h"
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow w;
    w.show();
    return app.exec();
}
