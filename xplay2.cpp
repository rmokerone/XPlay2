#include "xplay2.h"
#include "ui_xplay2.h"

#include "xdemux.h"
#include "xdecode.h"
#include "xresample.h"
#include "xaudioplay.h"

#include <iostream>
using namespace std;


#include <QThread>
#include "xaudiothread.h"
#include "xvideothread.h"


class TestThread :public QThread
{
public:
    XAudioThread at;
    XVideoThread vt;
    void init() {
        const char *filename = "v1080p.mp4";
        //const char *rtmp_url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
        cout << xdemux.Open(filename)<<endl;
        cout << "video param: " << xdemux.CopyVPara()<<endl;
        cout << "audio param: " << xdemux.CopyAPara()<<endl;
        cout << "width: " << xdemux.width<<endl;
        cout << "heigt: " << xdemux.height<<endl;

        cout << "at.Open = " << at.Open(xdemux.CopyAPara(), xdemux.sampleRate, xdemux.channels) <<endl;
        vt.Open(xdemux.CopyVPara(), video, xdemux.width, xdemux.height);
        at.start();
        vt.start();
    }

    unsigned char *pcm = new unsigned char[1024 * 1024];
    void run(){
        for (;;){
            AVPacket *pkt = xdemux.Read();
            if (!pkt){
                break;
            }
            if (xdemux.isAudio(pkt)){
                // 如果是音频
                at.Push(pkt);
            }else{
                // 如果是视频
                vt.Push(pkt);
                msleep(40);
            }
        }

    }
    // 测试Demux
    XDemux xdemux;
    XVideoWidget *video;
};

TestThread tt;
XPlay2::XPlay2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XPlay2)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);


    tt.video = ui->openGLWidget;
    tt.init();
    tt.start();
}

XPlay2::~XPlay2()
{
    delete ui;
}
