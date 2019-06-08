#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H


class XAudioPlay
{
public:
    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;
    // 打开一个音频播放
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual bool Write(const unsigned char *data, int datasize) = 0;
    virtual int GetFree() = 0;
    virtual void setPause(bool isPause) = 0;
    // 返回缓冲中还没有播放的时间 单位ms
    virtual long long GetNoPlayMs() = 0;
    // 一秒音频的字节大小
    double sizeps = 0;



    static XAudioPlay *Get();
    XAudioPlay();
    virtual ~XAudioPlay();
};

#endif // XAUDIOPLAY_H
