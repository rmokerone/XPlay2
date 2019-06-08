#ifndef XPLAY2_H
#define XPLAY2_H

#include <QWidget>

namespace Ui {
class XPlay2;
}

class XPlay2 : public QWidget
{
    Q_OBJECT

public:
    explicit XPlay2(QWidget *parent = 0);
    ~XPlay2();
    // 定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

public slots:
    void openFile();

private:
    Ui::XPlay2 *ui;
};

#endif // XPLAY2_H
