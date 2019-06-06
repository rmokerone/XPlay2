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


class TestThread :public QThread
{
public:
    XAudioThread at;
    void init() {
        const char *filename = "v1080p.mp4";
        //const char *rtmp_url = "rtmp://202.69.69.180:443/webcast/bshdlive-pc";
        cout << xdemux.Open(filename)<<endl;
        cout << "video param: " << xdemux.CopyVPara()<<endl;
        cout << "audio param: " << xdemux.CopyAPara()<<endl;
        cout << "width: " << xdemux.width<<endl;
        cout << "heigt: " << xdemux.height<<endl;
        //cout << "seek = " << xdemux.Seek(0.99) << endl;

        cout << "vdecode.Open() = " << vdecode.Open(xdemux.CopyVPara()) << endl;

        //cout << "adecode.Open() = " << adecode.Open(xdemux.CopyAPara()) << endl;


        video->setYUV420pParameters(xdemux.width, xdemux.height);

        //cout << "resample.Open() = " << resample.Open(xdemux.CopyAPara()) << endl;

        //XAudioPlay::Get()->channels = xdemux.channels;
        //XAudioPlay::Get()->sampleRate = xdemux.sampleRate;
        //cout << "XAudioPlay::Get()->Open() = "<< XAudioPlay::Get()->Open() <<endl;

        cout << "at.Open = " << at.Open(xdemux.CopyAPara(), xdemux.sampleRate, xdemux.channels) <<endl;
        at.start();
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
                /*
                adecode.Send(pkt);
                AVFrame *frame = adecode.Recv();
                //cout << "recv: " << frame << endl;
                // 先进行数据转码
                int len = resample.Resample(frame, pcm);
                //cout << "resample: "<< len << endl;

                while (len > 0){
                    // 获取有多少空间
                    //cout << "free = " << free << endl;
                    if (XAudioPlay::Get()->GetFree() >= len){
                        XAudioPlay::Get()->Write(pcm, len);
                        break;
                    }
                    //msleep(0.1);
                }
                */
                at.Push(pkt);
            }else{
                // 如果是视频
                vdecode.Send(pkt);
                AVFrame *frame = vdecode.Recv();
                if (frame){

                    data = QByteArray((const char *)frame->data[0], xdemux.height * xdemux.width);
                    data.append((const char *)frame->data[1], xdemux.height * xdemux.width / 4);
                    data.append((const char *)frame->data[2], xdemux.height * xdemux.width / 4);
                    av_frame_free(&frame);
                    //cout << "data size :"<< data.size() << endl;
                    video->setFrameData(data);
                    //video->Repaint(frame);
                    msleep(40);
                }

                //cout << "Video: " << frame <<endl;
            }
        }

    }
    // 测试Demux
    XDemux xdemux;
    XDecode vdecode;
    XDecode adecode;
    XResample resample;
    XVideoWidget *video;
    QByteArray temp_data;
    QByteArray data;
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
