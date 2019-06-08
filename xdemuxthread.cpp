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
    // 开放总的时间长度
    totalMs = xdemux->totalMs;

    mux.unlock();
    cout << "XDemuxThread::Open :"<<re<<endl;
    return re;
}

void  XDemuxThread::Start(){
    mux.lock();
    if (!xdemux) xdemux = new XDemux();
    if (!at) at = new XAudioThread();
    if (!vt) vt = new XVideoThread();
    QThread::start();
    if (vt) vt->start();
    if (at) at->start();
    mux.unlock();
}

void XDemuxThread::run(){
    while (!isExit){



        mux.lock();

        if (isPause){
            mux.unlock();
            msleep(1);
            continue;
        }

        if (!xdemux){
            mux.unlock();
            msleep(1);
            continue;
        }
        // 同步
        if (vt && at){
            pts = at->pts;
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
            //msleep(40);

        }
        mux.unlock();

    }
}

// 停止线程 释放资源
void XDemuxThread::Close()
{
    isExit = true;
    wait();

    if (vt){
        vt->Close();
    }
    if (at){
        at->Close();
    }
    mux.lock();
    delete vt;
    delete at;
    vt = NULL;
    at = NULL;
    mux.unlock();
}

void XDemuxThread::Clear()
{
    mux.lock();
    if (xdemux) xdemux->Clear();
    if (at) at->Clear();
    if (vt) vt->Clear();
    mux.unlock();
}

void XDemuxThread::Seek(double pos)
{
    // 清理缓存
    Clear();

    mux.lock();
    bool currentPause = isPause;
    mux.unlock();


    // 暂停其他线程
    setPause(true);

    mux.lock();
    if (xdemux)
        xdemux->Seek(pos);

    // 实际要显示的位置
    long long seekPts = pos * xdemux->totalMs;

    while (!isExit){
        AVPacket *pkt = xdemux->Read();
        if (!pkt) break;
        if (xdemux->isAudio(pkt)){
            // 为音频
            XFreePacket(&pkt);
            continue;
        }else{
            // 为视频
            bool re = vt->decode->Send(pkt);
            if (!re){
                break;
            }else{
                // 解码出AVFrame
                AVFrame *frame = vt->decode->Recv();
                if (!frame){
                    continue;
                }else{
                    if (frame->pts >= seekPts){
                        // 更新时间戳
                        pts = frame->pts;
                        vt->call->Repaint(frame);
                        break;
                    }
                }
                av_frame_free(&frame);
            }
        }
    }
    mux.unlock();
    // 一开始不是暂停状态 启动其他线程
    if (currentPause == false)
        setPause(false);
}

// 设置暂停
void XDemuxThread::setPause(bool isPause)
{
    mux.lock();
    this->isPause = isPause;
    if (at) at->setPause(isPause);
    if (vt) vt->setPause(isPause);
    mux.unlock();
}
