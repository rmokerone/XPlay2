#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

#include <QThread>
#include "xdemux.h"
#include "xdecode.h"
#include "xaudiothread.h"
#include "xvideothread.h"
#include <mutex>
#include <iostream>

#include "ivideocall.h"

class XDemuxThread: public QThread
{
public:
    virtual bool Open(const char *url, IVideoCall *call);
    // 启动线程
    virtual void Start();
    //
    void run();
    // 关闭线程 清理资源
    virtual void Close();

    // 清理缓冲
    virtual void Clear();

    virtual void Seek(double pos);

    void setPause(bool isPause);
    bool isPause = false;

    XDemuxThread();
    ~XDemuxThread();

    long long pts = 0;
    long long totalMs  = 0;
    bool isExit = false;

protected:
    std::mutex mux;
    XDemux *xdemux = 0;
    XAudioThread *at = 0;
    XVideoThread *vt = 0;
};

#endif // XDEMUXTHREAD_H
