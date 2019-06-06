#ifndef IVIDEOCALL_H
#define IVIDEOCALL_H

struct AVFrame;

class IVideoCall
{
public:
    virtual void Init(int width, int height) = 0;
    virtual void Repaint(AVFrame *frame) = 0;
};

#endif // IVIDEOCALL_H
