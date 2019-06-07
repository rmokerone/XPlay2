#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H
#include <QThread>
#include <mutex>
#include <list>

#include "xaudioplay.h"
#include "xdecode.h"
#include "xresample.h"

extern "C" {
#include "libavcodec/avcodec.h"
}


class XAudioThread: public QThread
{
public:
    // 当前音频播放对应的pts
    long long pts = 0;
    // 打开，不管成功与否 都清理para
    virtual bool Open(AVCodecParameters *para, int sampleRate, int channels);
    virtual void Push(AVPacket *pkt);
    void run();
    XAudioThread();
    virtual ~XAudioThread();
    // 最大队列
    int maxList = 100;
    bool isExit = false;
protected:
    std::list <AVPacket*> packs;
    std::mutex mux;
    XDecode *decode = 0;
    XAudioPlay *ap = 0;
    XResample *res = 0;
};

#endif // XAUDIOTHREAD_H
