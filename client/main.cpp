#include <QApplication>
#include "client_window.h"
#include "client_config.h"
#include <QStandardPaths>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow w;
    w.show();
    return app.exec();
}
