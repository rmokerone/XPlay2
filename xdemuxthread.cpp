#include "xdemuxthread.h"

using namespace std;

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
    isExit = true;
    wait();
}


bool XDemuxThread::Open(const char *url, IVideoCall *call){
    if (url == 0 || url[0] == '\0'){
        return false;
    }
    mux.lock();
    if (!xdemux) xdemux = new XDemux();
    if (!at) at = new XAudioThread();
    if (!vt) vt = new XVideoThread();

    int re = xdemux->Open(url);
    if (!re){
        mux.unlock();
        cout << "xdemux-Open failed"<<endl;
        return false;
    }
    // 创建视频线程 打开视频解码器和处理线程
    if (!vt->Open(xdemux->CopyVPara(), call, xdemux->width, xdemux->height)){
        re = false;
        cout << "vt-Open failed"<<endl;
    }
    // 创建音频现场 打开音频解码器和处理线程
    if (!at->Open(xdemux->CopyAPara(), xdemux->sampleRate, xdemux->channels)){
        re = false;
        cout << "at-Open failed"<<endl;
    }
    mux.unlock();
    cout << "XDemuxThread::Open :"<<re<<endl;
    return re;
}

void  XDemuxThread::Start(){
    mux.lock();
    this->start();
    if (vt) vt->start();
    if (at) at->start();
    mux.unlock();
}

void XDemuxThread::run(){
    while (!isExit){
        mux.lock();
        if (!xdemux){
            mux.unlock();
            msleep(5);
            continue;
        }
        // 同步
        if (vt && at){
            vt->syncpts = at->pts;
        }

        AVPacket *pkt = xdemux->Read();
        if (!pkt){
            mux.unlock();
            msleep(5);
            continue;
        }
        // 判断数据是否为音频
        if (xdemux->isAudio(pkt)){
            // 如果为音频
            if (at)
                at->Push(pkt);
        }else{
            // 如果为视频
            if (vt)
                vt->Push(pkt);
            //msleep(30);
        }
        mux.unlock();

    }
}
