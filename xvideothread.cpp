#include "xvideothread.h"
#include <iostream>

using namespace std;
XVideoThread::XVideoThread()
{

}


XVideoThread::~XVideoThread()
{

}

void XVideoThread::setPause(bool isPause)
{
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}

// 打开 不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height){
    syncpts = 0;
    if (!para) return false;
    Clear();
    vmux.lock();
    // 初始化显示窗口
    this->call = call;
    if (call){
        call->Init(width, height);
    }
    vmux.unlock();

    bool re = true;
    if (!decode->Open(para)){
        cout << "video XDecode open failed!" << endl;
        re = false;
    }

    cout << "XVideoThread::Open :"<< re << endl;
    return re;

}


void XVideoThread::run(){

    while(!isExit){
        vmux.lock();

        if (isPause){
            vmux.unlock();
            msleep(5);
            continue;
        }

        // 同步判断
        //cout << "syncpt"<<syncpts<<endl;
        //cout << "video pts"<<decode->pts<<endl;
        if (syncpts > 0 && syncpts < decode->pts){
            vmux.unlock();
            msleep(1);
            continue;
        }


        AVPacket *pkt = Pop();
        if (!pkt){
            vmux.unlock();
            msleep(1);
            continue;
        }

        cout << "video pts = "<< decode->pts <<endl;
        // 解码
        int re = decode->Send(pkt);
        if (!re){
            vmux.unlock();
            msleep(1);
            continue;
        }

        while (!isExit){
            AVFrame *frame = decode->Recv();
            if (!frame){
                break;
            }
            if (call){
                call->Repaint(frame);
            }
            msleep(1);
        }
        vmux.unlock();
        msleep(10);
    }
}

bool XVideoThread::RepaintPts(AVPacket *pkt, long long seekPts)
{
    vmux.lock();
    bool re  = decode->Send(pkt);
    if (!re){
        vmux.unlock();
        return false;
    }else{
        // 解码出AVFrame
        AVFrame *frame = decode->Recv();
        if (!frame){
            vmux.unlock();
            return false;
        }else{
            if (frame->pts >= seekPts){
                if (call) call->Repaint(frame);
                vmux.unlock();
                return true;
            }
        }
        av_frame_free(&frame);
    }
    vmux.unlock();
    return false;
}

