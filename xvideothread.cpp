#include "xvideothread.h"
#include <iostream>

using namespace std;
XVideoThread::XVideoThread()
{

}


XVideoThread::~XVideoThread()
{
    // 等待线程退出
    isExit = true;
    wait();
}

// 打开 不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height){
    syncpts = 0;
    if (!para) return false;
    mux.lock();
    // 初始化显示窗口
    this->call = call;
    if (call){
        call->Init(width, height);
    }
    // 打开解码器
    if (!decode) decode = new XDecode();
    bool re = true;
    if (!decode->Open(para)){
        cout << "video XDecode open failed!" << endl;
        re = false;
    }
    mux.unlock();
    cout << "XVideoThread::Open :"<< re << endl;
    return re;

}
void XVideoThread::Push(AVPacket *pkt){
    if (!pkt)
        return;
    // 写入数据到队列
    mux.lock();
    // 阻塞
    while (!isExit){
        if (packs.size() < maxList){
            packs.push_back(pkt);
            break;
        }
        msleep(1);
    }
    mux.unlock();
}
void XVideoThread::run(){

    while(!isExit){
        mux.lock();
        // 如果没有数据
        if (packs.empty() || !decode ){
            mux.unlock();
            msleep(1);
            continue;
        }

        // 同步判断
        if (syncpts > 0 && syncpts < decode->pts){
            mux.unlock();
            msleep(1);
            continue;
        }
        cout << "video pts = "<< decode->pts <<endl;

        AVPacket *pkt = packs.front();
        packs.pop_front();
        // 解码
        int re = decode->Send(pkt);
        if (!re){
            mux.unlock();
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
        mux.unlock();
        msleep(10);
    }
}
