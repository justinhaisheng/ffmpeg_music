//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSAUDIO_H
#define MUSIC_HSAUDIO_H

extern "C"
{
#include "libavcodec/avcodec.h"
};

class HsAudio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;

public:
    HsAudio();
    ~HsAudio();
};


#endif //MUSIC_HSAUDIO_H
