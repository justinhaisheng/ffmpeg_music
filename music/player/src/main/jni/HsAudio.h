//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSAUDIO_H
#define MUSIC_HSAUDIO_H

#include "HsPlaystatus.h"
#include "HsQueue.h"
#include "HsCalljava.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class HsAudio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    int sample_rate = -1;

    HsQueue* queue = NULL;
    HsPlaystatus* playstatus = NULL;
    HsCalljava* calljava = NULL;

    pthread_t thread_play;
    uint8_t* resample_data = NULL;//重采样后的数据
    //size_t   resample_data_size = 0;

    //opensl_es
    // 引擎接口
     SLObjectItf engineObject = NULL;
     SLEngineItf engineEngine = NULL;
    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;
public:
    HsAudio(HsPlaystatus* playstatus,HsCalljava* calljava,AVCodecParameters *codecpar,int streamIndex);
    ~HsAudio();

    SLuint32 getCurrentSampleRateForOpensles(int sample_rate);
    void initOpenSLES();
    void play();
    void pause();
    void resume();
    int resampleAudio();
};


#endif //MUSIC_HSAUDIO_H
