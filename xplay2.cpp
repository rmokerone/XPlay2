#include "xplay2.h"
#include "ui_xplay2.h"

#include <QThread>

#include "xdemuxthread.h"

// 一定不能再XPlay2函数内，否则run()还未运行，便解构了。
XDemuxThread dt;

XPlay2::XPlay2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XPlay2)
{
    const char *filename = "v1080p.mp4";
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);


    dt.Open(filename, ui->openGLWidget);
    dt.Start();
}

XPlay2::~XPlay2()
{
    delete ui;
}
