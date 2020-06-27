//
// Created by haisheng.lu on 2020/6/27.
//

#ifndef MUSIC_HSPCMBEAN_H
#define MUSIC_HSPCMBEAN_H


#include "SoundTouch.h"
using namespace soundtouch;

class HsPcmBean{
public:
    uint8_t* buffer;
    int buffsize;

public:
    HsPcmBean(SAMPLETYPE *buffer, int size);
    ~HsPcmBean();
};


#endif //MUSIC_HSPCMBEAN_H
