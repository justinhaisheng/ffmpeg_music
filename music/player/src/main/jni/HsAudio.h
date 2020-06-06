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

#include "SoundTouch.h"
using namespace soundtouch;

class HsAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    int sample_rate = -1;

    HsQueue* queue = NULL;
    HsPlaystatus* playstatus = NULL;
    HsCalljava* calljava = NULL;

    pthread_t thread_play;
    uint8_t* resample_data = NULL;//重采样后的数据
    int resample_nb = 0;//采样个数
    //size_t   resample_data_size = 0;
    size_t total_duration = 0;//总时长
    AVRational FRAME_TIME_BASE;//frame的单位时间
    double play_clock = 0;//总的播放时长
    double play_frame_now_time;//当前frame的时间
    double play_last_clock = 0;//记录上一次的播放时长
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
    //volume
    SLVolumeItf volumeObject = NULL;
    //mute
    SLMuteSoloItf muteObject = NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    //SoundTouch
    SoundTouch* soundTouch = NULL;
    SAMPLETYPE* sampleBuffer = NULL;
    uint8_t* sound_out_buffer = NULL;
    bool sound_finished = true;

public:
    HsAudio(HsPlaystatus* playstatus,HsCalljava* calljava,AVCodecParameters *codecpar,int streamIndex);
    ~HsAudio();

    SLuint32 getCurrentSampleRateForOpensles(int sample_rate);
    void initOpenSLES();
    void play();
    void pause();
    void resume();
    int resampleAudio();
    void stop();
    void release();
    void setMute(int mute);
    void setVolume(int volume);
    int getSoundTouchData();
    void setPitch(float pitch);
    void setSpeed(float speed);
    int getPCMDB(uint8_t* pcmData,size_t pcmSize);

};


#endif //MUSIC_HSAUDIO_H
