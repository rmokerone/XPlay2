#ifndef XRESAMPLE_H
#define XRESAMPLE_H
#include <iostream>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

class XResample
{
public:
    XResample();
    ~XResample();
    // 输出参数和输入参数一致
    virtual bool Open(AVCodecParameters *para, bool isClearPara = false);
    virtual void Close();
    // 返回重采样后大小
    virtual int Resample(AVFrame *indata, unsigned char *outdata);
    // 输出格式
    AVSampleFormat outFormat = AV_SAMPLE_FMT_S16;
    //AV_SAMPLE_FMT_S16

protected:
    std::mutex mux;
    SwrContext *actx = 0;
};

#endif // XRESAMPLE_H
