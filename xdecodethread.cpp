#include "xdecodethread.h"

XDecodeThread::XDecodeThread()
{
    // 打开解码器
    if (!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread()
{
    // 等待线程退出
    isExit = true;
    wait();
}

void XDecodeThread::Push(AVPacket *pkt){
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

// 取出一帧数据，并出栈，如果没有数据返回NULL
AVPacket *XDecodeThread::Pop(){
    mux.lock();
    if (packs.empty()){
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}
