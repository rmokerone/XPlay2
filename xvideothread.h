#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

#include <QThread>
#include <list>
#include <mutex>
#include "xdecode.h"
#include "ivideocall.h"
#include "xdecodethread.h"

extern "C" {
#include "libavcodec/avcodec.h"
}


class XVideoThread: public XDecodeThread
{
public:
    // 打开 不管成功与否都清理
    virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
    void run();

    XVideoThread();
    virtual ~XVideoThread();

    // 同步时间 由外部传入
    long long syncpts = 0;

    void setPause(bool isPause);
    bool isPause = false;
    IVideoCall *call = 0;
protected:
    std::mutex vmux;

};

#endif // XVIDEOTHREAD_H
