#include "xresample.h"



using namespace std;


XResample::XResample()
{

}

XResample::~XResample()
{

}

// 会释放para
bool XResample::Open(AVCodecParameters *para, bool isClearPara){
    if (!para) return false;

    // 音频重采样 上下文初始化
    mux.lock();
    if (!actx){
        actx = swr_alloc();
    }
    // 如果actx 为NULL 会自动分配空间
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(2),
                              outFormat,
                              para->sample_rate,
                              av_get_default_channel_layout(para->channels),
                              (AVSampleFormat)para->format,
                              para->sample_rate,
                              0,0
                              );
    if (isClearPara)
        avcodec_parameters_free(&para);
    int re = swr_init(actx);
    mux.unlock();
    if (re != 0){

        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init failed! :" << buf << endl;
        return false;
    }

    return true;
}

void XResample::Close(){
    mux.lock();
    if (actx){
        swr_free(&actx);
    }
    mux.unlock();
}

// indata 是在哪里释放 ， 不管成功与否都释放indata空间
int XResample::Resample(AVFrame *indata, unsigned char *outdata){
    if (!indata){
        return 0;
    }
    if (!outdata){
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = {0};
    data[0] = outdata;
    //cout << "start convert"<<endl;
    int re = swr_convert(actx,
                         data, indata->nb_samples,
                         (const uint8_t**)indata->data, indata->nb_samples);
    //cout << "end convert"<<endl;
    int outSize = re * indata->channels * av_get_bytes_per_sample(outFormat);
    av_frame_free(&indata);
    if (re <= 0){
        return re;
    }
    return outSize;
}
