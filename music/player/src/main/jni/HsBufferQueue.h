//
// Created by ywl on 2017-12-3.
//

#ifndef WLPLAYER_BUFFERQUEUE_H
#define WLPLAYER_BUFFERQUEUE_H

#include "deque"
#include "HsPlaystatus.h"
#include "HsPcmBean.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include "pthread.h"
};

using namespace std;

class HsBufferQueue {

public:
    deque<HsPcmBean *> queueBuffer;
    pthread_mutex_t mutexBuffer;
    pthread_cond_t condBuffer;
    HsPlaystatus *hsPlayStatus = NULL;

public:
    HsBufferQueue(HsPlaystatus *playStatus);
    ~HsBufferQueue();
    int putBuffer(SAMPLETYPE *buffer, int size);
    int getBuffer(HsPcmBean **pcmBean);
    int clearBuffer();

    void release();
    int getBufferSize();

    int noticeThread();
};


#endif //WLPLAYER_BUFFERQUEUE_H
