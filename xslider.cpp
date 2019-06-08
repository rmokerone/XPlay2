#include "xslider.h"
#include <QDebug>
#include <QMouseEvent>

XSlider::XSlider(QWidget *parent) : QSlider(parent)
{
}

XSlider::~XSlider()
{

}

void XSlider::mousePressEvent(QMouseEvent *ev)
{
    //qDebug() << ev->pos();
    double pos = (double)ev->pos().x() / (double)width();
    setValue(pos * this->maximum());
    // 原有事件处理
    QSlider::mousePressEvent(ev);
}
