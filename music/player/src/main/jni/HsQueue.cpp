//
// Created by haisheng on 2020/5/12.
//

#include "HsQueue.h"

HsQueue::HsQueue(HsPlaystatus *playstatus) {
    this->playstatus = playstatus;
    pthread_mutex_init(&this->mutexPacket,NULL);
    pthread_cond_init(&this->condPacket,NULL);
}

HsQueue::~HsQueue() {
    clearQueue();
    pthread_mutex_destroy(&this->mutexPacket);
    pthread_cond_destroy(&this->condPacket);
    if (playstatus){
        playstatus = NULL;
    }
}

int HsQueue::putPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);

    this->queuePacket.push(packet);
    if(LOG_DEBUG)
    {
        LOGD("放入一个AVpacket到队里里面， 个数为：%d", queuePacket.size());
    }
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int HsQueue::getPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    while (this->playstatus && !this->playstatus->exit){
        if (this->queuePacket.size() > 0){
            AVPacket *avPacket = this->queuePacket.front();
            if (av_packet_ref(packet,avPacket) == 0){
                this->queuePacket.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            if(LOG_DEBUG)
            {
                LOGI("从队列里面取出一个AVpacket，还剩下 %d 个", queuePacket.size());
            }
            break;
        }else{
            pthread_cond_wait(&condPacket,&mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int HsQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = this->queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}


void HsQueue::clearQueue() {
    if(LOG_DEBUG)
    {
        LOGI("清除队列");
    }
    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);
    while (!queuePacket.empty())
    {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
}

