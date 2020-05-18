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
};


class HsFFmpeg {
public:
    char* url = NULL;
    HsCalljava *calljava = NULL;
    HsPlaystatus* playstatus = NULL;
    AVFormatContext* pFormatContext = NULL;
    pthread_t decodeThread;
    HsAudio *audio = NULL;
public:
    HsFFmpeg(HsCalljava* calljava,HsPlaystatus* playstatus, const char* url);
    ~HsFFmpeg();

    void resume();
    void pause();
    void prepare();
    void start();
    void decodeFFmpegThread();
};


#endif //MUSIC_HSFFMPEG_H
