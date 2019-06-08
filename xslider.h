#ifndef XSLIDER_H
#define XSLIDER_H

#include <QWidget>
#include <QSlider>

class XSlider : public QSlider
{
    Q_OBJECT
public:
    explicit XSlider(QWidget *parent = nullptr);
    ~XSlider();
    void mousePressEvent(QMouseEvent *e);

signals:

public slots:
};

#endif // XSLIDER_H
