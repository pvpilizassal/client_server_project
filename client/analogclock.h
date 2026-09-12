#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

// Адаптировано из Qt-примера "Analog Clock"
// Copyright (C) 2016 The Qt Company Ltd
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
//
// Отличия от оригинала:
//  - убрана секундная стрелка
//  - удалён внутренний QTimer — время задаётся извне через setTime(),
//    единственным источником истины остаётся сервер;
//  - в paintEvent используется сохранённое время вместо QTime::currentTime()

#include <QWidget>
#include <QTime>

class AnalogClock : public QWidget
{
    Q_OBJECT
public:
    explicit AnalogClock(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void setTime(const QTime& time);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTime m_time;  // по умолчанию 00:00
};

#endif // ANALOGCLOCK_H
