#include "xplay2.h"
#include "ui_xplay2.h"

#include <QThread>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "xdemuxthread.h"

// 一定不能再XPlay2函数内，否则run()还未运行，便解构了。
XDemuxThread dt;

XPlay2::XPlay2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XPlay2)
{

    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    dt.Start();
    // 开启滑动条时间显示定时器
    startTimer(40);
}

void XPlay2::openFile(){
    const char *filename = "v1080p.mp4";
    const char *url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
    //dt.Open(filename, ui->openGLWidget);
    cout << "Open file success!"<<endl;
    // 选择文件
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    qDebug() << "name = " << name;
    if (name.isEmpty()) {return;}
    this->setWindowTitle(name);
    if (dt.Open(name.toLocal8Bit(), ui->openGLWidget)){
        cout << "Open file success!"<<endl;
    }else{
        QMessageBox::information(this, "ERROR", "Open file failed");
        return;
    }
}

XPlay2::~XPlay2()
{
    dt.Close();
    delete ui;
}

// 定时器 滑动条显示
void XPlay2::timerEvent(QTimerEvent *e)
{
    long long totalMs = dt.totalMs;
    if (totalMs > 0){
        double pos = (double)dt.pts / (double)totalMs;
        int v = ui->playSlider->maximum() * pos;
        ui->playSlider->setValue(v);
    }
}
