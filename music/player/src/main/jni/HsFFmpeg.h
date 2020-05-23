//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSFFMPEG_H
#define MUSIC_HSFFMPEG_H

#include <pthread.h>
#include "HsCalljava.h"
#include "HsAudio.h"
extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/time.h>
};


class HsFFmpeg {
public:
    char* url = NULL;
    HsCalljava *calljava = NULL;
    HsPlaystatus* playstatus = NULL;
    AVFormatContext* pFormatContext = NULL;
    pthread_t decode_thread;
    pthread_t start_thread;
    pthread_mutex_t decode_mutex;
    pthread_mutex_t seek_mutex;
    HsAudio *audio = NULL;
    bool decode_exit;
public:
    HsFFmpeg(HsCalljava* calljava,HsPlaystatus* playstatus, const char* url);
    ~HsFFmpeg();

    void resume();
    void pause();
    void prepare();
    void start();
    void decodeFFmpegThread();
    void startFFmpegThread();
    void release();

    void seek(int64_t secs);
};


#endif //MUSIC_HSFFMPEG_H
