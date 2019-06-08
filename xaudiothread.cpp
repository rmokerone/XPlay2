#include "xaudiothread.h"
#include <iostream>
using namespace std;

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels){
    pts = 0;
    if (!para)
        return false;
    Clear();

    amux.lock();

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
    amux.unlock();
    cout << "XAudioThread::Open :"<< re <<endl;
    return re;
}

void XAudioThread::Close(){
    XDecodeThread::Close();
    if (res){
        res->Close();
        amux.lock();
        delete res;
        res = NULL;
        amux.unlock();
    }

    if (ap){
        ap->Close();
        amux.lock();
        ap = NULL;
        amux.unlock();
    }
}

void XAudioThread:: Clear()
{
    XDecodeThread::Clear();
    amux.lock();
    if (ap) ap->Clear();
    amux.unlock();
}


void XAudioThread::run() {
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while(!isExit){
        amux.lock();
        // 如果没有数据

        if (isPause){
            amux.unlock();
            msleep(5);
            continue;
        }

        AVPacket *pkt = Pop();
        // 解码
        int re = decode->Send(pkt);
        if (!re){
            amux.unlock();
            msleep(1);
            continue;
        }
        // 可能一次send 多次Recv
        while (!isExit){
            AVFrame *frame = decode->Recv();

            // 减去缓冲中未播放的ms
            pts = decode->pts - ap->GetNoPlayMs();
            cout << "audio pts = "<< pts <<endl;

            if (!frame) break;
            // 重采样
            int size = res->Resample(frame, pcm);
            // 播放音频
            while (!isExit){
                if (size <= 0){
                    break;
                }
                // 缓冲未播完 空间不够
                if (ap->GetFree() < size || isPause){
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
        msleep(10);
    }
    delete pcm;
}

XAudioThread::XAudioThread()
{
    if (!res){
        res = new XResample();
    }
    if (!ap){
        ap = XAudioPlay::Get();
    }
}

XAudioThread::~XAudioThread()
{
}

void XAudioThread::setPause(bool isPause)
{
    this->isPause = isPause;
    if (ap){
        ap->setPause(isPause );
    }
}
