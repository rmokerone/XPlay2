#include "xdecode.h"

using namespace std;


extern "C" {
#include "libavcodec/avcodec.h"
}


XDecode::XDecode()
{

}

XDecode::~XDecode(){

}



// 打开解码器 并释放空间 不管成功与否
bool XDecode::Open(AVCodecParameters *para){
    if (!para)
        return false;

    Close();
    // 找到解码器
    AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
    if (!vcodec){
        // 找不到解码器，提示错误，并退出
        cout << "can't find the codec"<<endl;
        avcodec_parameters_free(&para);
        return false;
    }else{
        cout << "find the video codec id = "<<para->codec_id<<endl;
    }

    mux.lock();

    // 创建解码器上下文
    codec = avcodec_alloc_context3(vcodec);
    // 配置解码器上下文
    avcodec_parameters_to_context(codec, para);
    codec->thread_count = 8;

    // 打开解码器上下文
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0){
        avcodec_free_context(&codec);
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) -1);
        cout << "avcodec_open2 failure: " << buf<<endl;
        avcodec_parameters_free(&para);
        return false;
    }else{
        cout << "video codec open success!"<<endl;
    }
    mux.unlock();
    avcodec_parameters_free(&para);
    return true;
}

// 发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
bool XDecode::Send(AVPacket *pkt){
    if (!pkt || pkt->size <= 0 || !pkt->data)
        return false;
    mux.lock();
    if (!codec){
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    mux.unlock();
    // 释放AVPacket
    av_packet_free(&pkt);
    if (re != 0){
        return false;
    }
    return true;
}

// 获取解码后的数据，一次Send可能需要多次Recv，获取缓冲中的数据Send NULL 再Recv多次
// 每次复制一份 由调用者释放 av_frame_free
AVFrame *XDecode::Recv(){
    mux.lock();
    if (!codec){
        mux.unlock();
        return NULL;
    }

    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mux.unlock();
    if (re != 0){
        av_frame_free(&frame);
        return NULL;
    }

    //cout << "linesize[0]: "<< frame->linesize[0] << " " << flush;
    return frame;
}


void XDecode::Close(){
    mux.lock();
    if (codec){
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();
}

void XDecode::Clear(){
    mux.lock();

    // 清理解码缓冲
    if (codec){
        avcodec_flush_buffers(codec);
    }

    mux.unlock();
}
