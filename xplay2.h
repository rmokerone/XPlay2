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

private:
    Ui::XPlay2 *ui;
};

#endif // XPLAY2_H
