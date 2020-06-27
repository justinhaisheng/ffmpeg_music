//
// Created by ywl on 2017-12-3.
//

#include "HsBufferQueue.h"
#include "AndroidLog.h"

HsBufferQueue::HsBufferQueue(HsPlaystatus *playStatus) {
    hsPlayStatus = playStatus;
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);
}

HsBufferQueue::~HsBufferQueue() {
    hsPlayStatus = NULL;
    pthread_mutex_destroy(&mutexBuffer);
    pthread_cond_destroy(&condBuffer);
    if(LOG_DEBUG)
    {
        LOGE("BufferQueue 释放完了");
    }
}

void HsBufferQueue::release() {

    if(LOG_DEBUG)
    {
        LOGE("BufferQueue::release");
    }
    noticeThread();
    clearBuffer();

    if(LOG_DEBUG)
    {
        LOGE("BufferQueue::release success");
    }
}

int HsBufferQueue::putBuffer(SAMPLETYPE *buffer, int size) {
    pthread_mutex_lock(&mutexBuffer);
    HsPcmBean *pcmBean = new HsPcmBean(buffer, size);
    queueBuffer.push_back(pcmBean);
    pthread_cond_signal(&condBuffer);
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int HsBufferQueue::getBuffer(HsPcmBean **pcmBean) {

    pthread_mutex_lock(&mutexBuffer);

    while(hsPlayStatus != NULL && !hsPlayStatus->exit)
    {
        if(queueBuffer.size() > 0)
        {
            *pcmBean = queueBuffer.front();
            queueBuffer.pop_front();
            break;
        } else{
            if(!hsPlayStatus->exit)
            {
                pthread_cond_wait(&condBuffer, &mutexBuffer);
            }
        }
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int HsBufferQueue::clearBuffer() {

    pthread_cond_signal(&condBuffer);
    pthread_mutex_lock(&mutexBuffer);
    while (!queueBuffer.empty())
    {
        HsPcmBean *pcmBean = queueBuffer.front();
        queueBuffer.pop_front();
        delete(pcmBean);
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int HsBufferQueue::getBufferSize() {
    int size = 0;
    pthread_mutex_lock(&mutexBuffer);
    size = queueBuffer.size();
    pthread_mutex_unlock(&mutexBuffer);
    return size;
}


int HsBufferQueue::noticeThread() {
    pthread_cond_signal(&condBuffer);
    return 0;
}

