#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H
#include <QThread>
#include <mutex>
#include <list>

#include "xaudioplay.h"
#include "xresample.h"
#include "xdecodethread.h"

extern "C" {
#include "libavcodec/avcodec.h"
}


class XAudioThread: public XDecodeThread
{
public:
    // 当前音频播放对应的pts
    long long pts = 0;
    // 打开，不管成功与否 都清理para
    virtual bool Open(AVCodecParameters *para, int sampleRate, int channels);
    // 停止线程 清理资源
    virtual void Close();

    virtual void Clear();

    void run();
    XAudioThread();
    virtual ~XAudioThread();

    void setPause(bool isPause);
    bool isPause = false;
protected:
    std::list <AVPacket*> packs;
    std::mutex amux;
    XAudioPlay *ap = 0;
    XResample *res = 0;
};

#endif // XAUDIOTHREAD_H
