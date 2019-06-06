#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

#include <QThread>
#include "xdemux.h"
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
    XDemuxThread();
    ~XDemuxThread();
    bool isExit = false;
protected:
    std::mutex mux;
    XDemux *xdemux = 0;
    XAudioThread *at = 0;
    XVideoThread *vt = 0;
};

#endif // XDEMUXTHREAD_H
