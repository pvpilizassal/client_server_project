// Адаптировано из Qt-примера "Analog Clock"
// Copyright (C) 2016 The Qt Company Ltd
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "analogclock.h"

#include <QPainter>
#include <QPaintEvent>

AnalogClock::AnalogClock(QWidget *parent)
    : QWidget(parent)
    , m_time(0, 0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(minimumSizeHint());
}

QSize AnalogClock::sizeHint() const
{
    return QSize(240, 240);
}

QSize AnalogClock::minimumSizeHint() const
{
    return QSize(120, 120);
}

void AnalogClock::setTime(const QTime& time)
{
    if (!time.isValid()) {
        return;
    }
    if (m_time == time) {
        return;
    }
    m_time = time;
    update();
}

void AnalogClock::paintEvent(QPaintEvent * /*event*/)
{
    // координаты стрелок заданы в логическом поле 200×200 (радиус 100)
    // масштабирование painter приводит их к фактическому размеру виджета
    static const QPoint hourHand[4] = {
        QPoint( 5,  14),
        QPoint(-5,  14),
        QPoint(-4, -71),
        QPoint( 4, -71)
    };
    static const QPoint minuteHand[4] = {
        QPoint( 4,  14),
        QPoint(-4,  14),
        QPoint(-3, -89),
        QPoint( 3, -89)
    };

    const QColor hourColor(palette().color(QPalette::Text));
    const QColor minuteColor(palette().color(QPalette::Text));

    const int side = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    // часовая стрелка
    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);
    {
        painter.save();
        painter.rotate(30.0 * (m_time.hour() + m_time.minute() / 60.0));
        painter.drawConvexPolygon(hourHand, 4);
        painter.restore();
    }

    // часовые деления (12 шт толстые)
    for (int i = 0; i < 12; ++i) {
        painter.drawRect(73, -3, 16, 6);
        painter.rotate(30.0);
    }

    // минутная стрелка
    painter.setBrush(minuteColor);
    {
        painter.save();
        painter.rotate(6.0 * m_time.minute());
        painter.drawConvexPolygon(minuteHand, 4);
        painter.restore();
    }

    // минутные деления (60 шт тонкие)
    painter.setPen(minuteColor);
    for (int j = 0; j < 60; ++j) {
        painter.drawLine(92, 0, 96, 0);
        painter.rotate(6.0);
    }
}