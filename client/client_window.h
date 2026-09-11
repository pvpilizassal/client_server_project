#ifndef CLIENT_WINDOW_H
#define CLIENT_WINDOW_H

#include <QMainWindow>
#include <AnalogClock>

class client_window : public QMainWindow
{
    Q_OBJECT
public:
    explicit client_window(QWidget *parent = nullptr);


signals:
};

#endif // CLIENT_WINDOW_H
