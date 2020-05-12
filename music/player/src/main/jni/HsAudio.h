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
};

class HsAudio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;

    HsQueue* queue = NULL;
    HsPlaystatus* playstatus = NULL;

public:
    HsAudio(HsPlaystatus* playstatus);
    ~HsAudio();
};


#endif //MUSIC_HSAUDIO_H
