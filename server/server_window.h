#ifndef SERVER_WINDOW_H
#define SERVER_WINDOW_H

#include <QMainWindow>

class serverWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit serverWindow(QWidget *parent = nullptr);

signals:
};

#endif // SERVER_WINDOW_H
