#ifndef XDECODE_H
#define XDECODE_H

#include <iostream>

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;


extern void XFreePacket(AVPacket **pkt);

class XDecode
{
public:
    bool isAudio = false;
    // 当前解码到的pts
    long long pts;
    XDecode();
    virtual ~XDecode();

    // 打开解码器 并释放param空间
    virtual bool Open(AVCodecParameters *para);

    // 关闭
    virtual void Close();

    virtual void Clear();

    // 发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
    virtual bool Send(AVPacket *pkt);

    // 获取解码后的数据，一次Send可能需要多次Recv，获取缓冲中的数据Send NULL 再Recv多次
    // 每次复制一份 由调用者释放 av_frame_free
    virtual AVFrame *Recv();

protected:
    AVCodecContext *codec = 0;
    std::mutex mux;

};

#endif // XDECODE_H
