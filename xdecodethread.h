#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

#include <QThread>

class XDecodeThread: public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
};

#endif // XDECODETHREAD_H
