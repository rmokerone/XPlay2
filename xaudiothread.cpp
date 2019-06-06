#include "xaudiothread.h"
#include <iostream>
using namespace std;

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels){
    if (!para)
        return false;
    mux.lock();

    if (!decode){
        decode = new XDecode();
    }
    if (!res){
        res = new XResample();
    }
    if (!ap){
        ap = XAudioPlay::Get();
    }
    // 打开重采样部分
    bool re = true;
    if (!res->Open(para, false)){
        re = false;
        cout << "XResample Open failed"<<endl;
    }


    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if (!ap->Open()){
        re = false;
        cout << "XAudioPlay Open failed"<<endl;
    }

    if (!decode->Open(para)){
        re = false;
        cout << "XDecode Open failed"<<endl;
    }
    mux.unlock();
    cout << "XAudioThread::Open :"<< re <<endl;
    return re;
}

void XAudioThread::Push(AVPacket *pkt){
    if (!pkt)
        return;
    // 写入数据到队列

    // 阻塞
    while (!isExit){
        mux.lock();
        if (packs.size() < maxList){
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
    mux.unlock();
}

void XAudioThread::run() {
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while(!isExit){
        mux.lock();
        // 如果没有数据
        if (packs.empty() || !decode ||!res ||!ap){
            mux.unlock();
            msleep(1);
            continue;
        }

        AVPacket *pkt = packs.front();
        packs.pop_front();
        // 解码
        int re = decode->Send(pkt);
        if (!re){
            mux.unlock();
            msleep(1);
            continue;
        }
        // 可能一次send 多次Recv
        while (!isExit){
            AVFrame *frame = decode->Recv();
            if (!frame) break;
            // 重采样
            int size = res->Resample(frame, pcm);
            // 播放音频
            while (!isExit){
                if (size <= 0){
                    break;
                }
                // 缓冲未播完 空间不够
                if (ap->GetFree() < size){
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        mux.unlock();
    }
    delete pcm;
}

XAudioThread::XAudioThread()
{

}

XAudioThread::~XAudioThread()
{
    // 等待线程退出
    isExit = true;
    wait();
}
