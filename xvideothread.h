#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

#include <QThread>
#include <list>
#include <mutex>
#include "xdecode.h"

extern "C" {
#include "libavcodec/avcodec.h"
}


class XVideoThread: public QThread
{
public:
    // 打开 不管成功与否都清理
    virtual bool Open(AVCodecParameters *para);
    virtual void Push(AVPacket *pkt);
    void run();

    XVideoThread();
    virtual ~XVideoThread();
    // 最大队列数量
    int maxList = 100;
    bool isExit = false;

protected:
    std::list <AVPacket *> packs;
    std::mutex mux;
    XDecode *decode = 0;
};

#endif // XVIDEOTHREAD_H
