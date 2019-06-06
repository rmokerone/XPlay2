#ifndef XDEMUX_H
#define XDEMUX_H
#include <QDebug>
#include <mutex>

using namespace std;


extern "C" {
#include "libavformat/avformat.h"
}

struct AVFormatContext;
struct AVPacket;

class XDemux
{
public:
    // 打开媒体文件，或者流媒体 rtmp http rtsp
    virtual bool Open(const char *url);

    // 空间需要调用者释放，释放AVPacket对象空间 和 数据空间av_packet_free
    virtual AVPacket *Read();

    // 获取视频参数 返回的空间需要清理 avcodec_parameters_free
    virtual AVCodecParameters *CopyVPara();

    // 获取音频参数 返回的空间需要清理  avcodec_parameters_free
    virtual AVCodecParameters *CopyAPara();

    virtual bool isAudio(AVPacket *pkt);

    // seek 位置 pos 0.0 ~ 1.0
    virtual bool Seek(double pos);


    // 清理 清空读取的缓存
    virtual void Clear();

    // 关闭 关闭上下文
    virtual void Close();


    XDemux();
    virtual ~XDemux();
    // 媒体总时长 ms
    int totalMs = 0;
    int width = 0;
    int height = 0;

    // 音频相关参数
    int sampleRate = 0;
    int channels = 0;

protected:
    std::mutex mux;
    AVFormatContext  *ic = NULL;

    // 音视频索引，读取时区分音视频
    int videoStream = 0;
    int audioStream = 1;
};

#endif // XDEMUX_H
