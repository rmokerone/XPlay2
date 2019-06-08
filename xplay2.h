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

    // 窗口尺寸变化
    void resizeEvent(QResizeEvent *e);

    // 双击窗口 点击相应
    void mouseDoubleClickEvent(QMouseEvent *e);

    void setPause(bool isPlay);

public slots:
    void openFile();
    void PlayOrPause();
    void sliderPressSlot();
    void sliderReleaseSlot();

private:
    bool isSliderPressed = false;
    Ui::XPlay2 *ui;
};

#endif // XPLAY2_H
