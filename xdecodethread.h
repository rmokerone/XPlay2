#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <QThread>
#include <list>
#include <mutex>
#include "xdecode.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

class XDecodeThread: public QThread
{
public:
    virtual void Push(AVPacket *pkt);
    // 取出一帧数据，并出栈，如果没有数据返回NULL
    virtual AVPacket *Pop();
    XDecodeThread();
    virtual ~XDecodeThread();
    // 最大队列数量
    int maxList = 100;
    bool isExit = false;
protected:
    std::list <AVPacket *> packs;
    std::mutex mux;
    XDecode *decode = 0;
};

#endif // XDECODETHREAD_H
