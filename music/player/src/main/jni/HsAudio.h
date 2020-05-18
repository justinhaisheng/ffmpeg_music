//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSAUDIO_H
#define MUSIC_HSAUDIO_H

#include "HsPlaystatus.h"
#include "HsQueue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
};

class HsAudio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;

    HsQueue* queue = NULL;
    HsPlaystatus* playstatus = NULL;

    pthread_t thread_play;
    uint8_t* resample_data = NULL;//重采样后的数据
    size_t   resample_data_size = 0;
public:
    HsAudio(HsPlaystatus* playstatus);
    ~HsAudio();

    void play();
    int resampleAudio();
};


#endif //MUSIC_HSAUDIO_H
