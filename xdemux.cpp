#include "xdemux.h"

#include <iostream>
#include <mutex>

using namespace std;

extern "C" {
#include "libavcodec/avcodec.h"
}



static double r2d(AVRational r){
    return r.den == 0 ? 0 : (double) r.num / (double)r.den ;
}

XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if (isFirst){
        // 初始化封装库
        av_register_all();
        // 初始化网络库 （可以打开rtsp rtmp http协议的流媒体视频）
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

XDemux::~XDemux()
{

}


bool XDemux::Open(const char *url){
    Close();
    // 参数设置，比如rtsp的延时时间
    AVDictionary *opts = NULL;
    // 设置rtsp以tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    // 设置网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);
    mux.lock();
    int re = avformat_open_input(&ic, url, NULL, &opts);
    if (re  !=  0){
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) -1);
        qDebug() << "open: " << url << " failure " << buf;
        return false;
    }

    qDebug() << "open: " << url << " success! ";

    // 获取流信息
   re = avformat_find_stream_info(ic, 0);
   // 总时长 毫秒
   totalMs = ic->duration / (AV_TIME_BASE / 1000);
   qDebug() <<"totalMs: "<<totalMs;

   // 打印视频流详细信息
   av_dump_format(ic, 0, url, 0);

   // 获取音视频流信息，两种方法
   // 1。遍历
   for (unsigned int i = 0; i < ic->nb_streams; i++){
       AVStream *as = ic->streams[i];
       // 音频
       if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
           audioStream = i;
           sampleRate = as->codecpar->sample_rate;
           channels = as->codecpar->channels;
           qDebug() << "AUDIO INFO: "<< i;
           qDebug() << "SAMPLE_RATE: "<< as->codecpar->sample_rate;
           qDebug() << "FORMAT: "<< as->codecpar->format;
           qDebug() << "CHANNEL NUM: "<< as->codecpar->channels;
           qDebug() << "AUDIO Fps: " << r2d(as->avg_frame_rate);
           qDebug() << "CODEC ID: "<<as->codecpar->codec_id;

           // 一帧数据 单通道的样本数
           qDebug() << "一帧数据的大小" << as->codecpar->frame_size;

       }
       // 视频
       else if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
           videoStream = i;
           // 设置宽高
           width = as->codecpar->width;
           height = as->codecpar->height;
           qDebug() << "VIDEO INFO: "<< i;
           qDebug() << "Width: " << as->codecpar->width;
           qDebug() << "Height: " << as->codecpar->height;
           // 打印帧率 分数需要转换 注意：分母是否为0
           qDebug() << "VIDEO Fps: " << r2d(as->avg_frame_rate);

           qDebug() << "CODEC ID: "<<as->codecpar->codec_id;

       }
   }
   mux.unlock();


   // 2 使用av_find_best_stream
   /*
   videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
   qDebug() << "video stream codec id = " << ic->streams[videoStream]->codecpar->codec_id;

   audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
   qDebug() << "audio stream codec id = " << ic->streams[audioStream]->codecpar->codec_id;
   */


    return true;
}

AVPacket *XDemux::Read(){
    mux.lock();
    // 如果没有打开ic 就直接做Read了
    if (!ic){
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = av_packet_alloc();
    // 读取一帧，并分配空间
    int re = av_read_frame(ic, pkt);
    if (re != 0){
        mux.unlock();
        av_packet_free(&pkt);
        return NULL;
    }
    // pts 转换为ms
    pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    //qDebug() << "pts: " << pkt->pts << "stream_index: "<< pkt->stream_index;
    return pkt;
}

// 获取视频参数 返回的空间需要清理 avcodec_parameters_free
AVCodecParameters *XDemux::CopyVPara(){
    mux.lock();
    if (ic == NULL){
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();

    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    mux.unlock();
    return pa;
}

// 获取音频参数 返回的空间需要清理  avcodec_parameters_free
AVCodecParameters *XDemux::CopyAPara(){
    mux.lock();
    if (ic == NULL){
        mux.unlock();
        return NULL;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();

    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
    mux.unlock();
    return pa;
}

// seek 位置 pos 0.0 ~ 1.0
bool XDemux::Seek(double pos){
    mux.lock();
    if (ic == NULL){
        mux.unlock();
        return false;
    }
    // 清理读取缓冲
    avformat_flush(ic);
    long long seekPos = 0;
    // 时间换算
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if (re < 0){
        return false;
    }
    return true;
}


// 清空缓存
void XDemux::Clear(){
    mux.lock();
    if (!ic){
        mux.unlock();
        return ;
    }
    avformat_flush(ic);
    mux.unlock();
}

// 关闭
void XDemux::Close() {
    mux.lock();
    if (!ic){
        mux.unlock();
        return ;
    }
    avformat_close_input(&ic);
    // 总时长清空
    totalMs = 0;
    mux.unlock();
}

bool XDemux::isAudio(AVPacket *pkt){
    if (!pkt) return false;

    if (pkt->stream_index == videoStream){
        return false;
    }
    return true;
}
