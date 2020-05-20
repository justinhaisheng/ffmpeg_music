//
// Created by haisheng on 2020/5/12.
//

#ifndef MUSIC_HSQUEUE_H
#define MUSIC_HSQUEUE_H

#include <queue>
#include <pthread.h>
#include "AndroidLog.h"
#include "HsPlaystatus.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};

using namespace std;

class HsQueue {
public:
    queue<AVPacket* > queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    HsPlaystatus *playstatus = NULL;

public:
    HsQueue(HsPlaystatus *playstatus);
    ~HsQueue();

    int putPacket(AVPacket* packet);
    int getPacket(AVPacket* packet);

    int getQueueSize();
    void clearQueue();
};


#endif //MUSIC_HSQUEUE_H
