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
    XDecodeThread();
    virtual ~XDecodeThread();

    virtual void Push(AVPacket *pkt);
    // 取出一帧数据，并出栈，如果没有数据返回NULL
    virtual AVPacket *Pop();
    // 清理队列
    virtual void Clear();
    // 清理资源，停止线程
    virtual void Close();

    // 最大队列数量
    int maxList = 100;
    bool isExit = false;
    XDecode *decode = 0;
protected:
    std::list <AVPacket *> packs;
    std::mutex mux;

};

#endif // XDECODETHREAD_H
