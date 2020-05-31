//
// Created by haisheng on 2020/5/11.
//

#include "HsAudio.h"

HsAudio::HsAudio(HsPlaystatus* playstatus,HsCalljava* calljava,AVCodecParameters *codecpar,int streamIndex) {
    this->playstatus = playstatus;
    this->calljava = calljava;
    this->codecpar = codecpar;
    this->streamIndex = streamIndex;
    this->sample_rate = this->codecpar->sample_rate;

    queue = new HsQueue(this->playstatus);
    resample_data = static_cast<uint8_t *>(av_malloc(sample_rate * 2 * 2));//申请一秒的数据量大小
    //双通道 16位 yuv
    sampleBuffer = static_cast<SAMPLETYPE *>(malloc(sample_rate * 2 * 2 * 1));
    soundTouch = new SoundTouch();
    soundTouch->setSampleRate(sample_rate);
    soundTouch->setChannels(2);
    sound_finished = true;
    setPitch(1.0f);
    setSpeed(1.0f);
}

HsAudio::~HsAudio() {
    av_free(resample_data);
    free(sampleBuffer);
    sampleBuffer = NULL;
    resample_data = NULL;
    delete soundTouch;
    soundTouch = NULL;
}

void* decode_play(void* data){
    HsAudio* audio = static_cast<HsAudio *>(data);
    audio->initOpenSLES();
    pthread_exit(&audio->thread_play);
}

void HsAudio::play() {
    pthread_create(&thread_play,NULL,decode_play,this);
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void* context)
{
    HsAudio* audio = static_cast<HsAudio *>(context);
    if (audio){
        //int buffer_size= audio->resampleAudio(reinterpret_cast<void **>(NULL));
        int buffer_size = audio->getSoundTouchData();
        // for streaming playback, replace this test by logic to find and fill the next buffer
        if (buffer_size > 0) {
            //SLresult result;
            // enqueue another buffer
            (*audio->pcmBufferQueue)->Enqueue(audio->pcmBufferQueue, (char *)audio->resample_data,buffer_size);
            LOGE("Enqueue %d",buffer_size);
            // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
            // which for this code example would indicate a programming error
        }
    }
}

void HsAudio::initOpenSLES() {
    SLresult result;
    //第一步------------------------------------------
    // 创建引擎对象
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (void)result;
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (void)result;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    (void)result;
    //第二步-------------------------------------------
    // 创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void)result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void)result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    (void)result;
    if (SL_RESULT_SUCCESS == result) {
        LOGI("SL_RESULT_SUCCESS GetInterface(outputMixObject) == result");
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    // 第三步--------------------------------------------
    // 创建播放器
    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            getCurrentSampleRateForOpensles(this->sample_rate),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };

    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink audioSnk = {&outputMix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE,SL_IID_VOLUME,SL_IID_MUTESOLO};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk,3, ids, req);
    (void)result;
    // 初始化播放器
    result = (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (void)result;
    //得到接口后调用  获取Player接口
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);
    (void)result;

    //获取音量的接口
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_VOLUME,&volumeObject);
    (void)result;

    //muteObject
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_MUTESOLO,&muteObject);
    (void)result;

    //第四步---------------------------------------
    // 创建缓冲区和回调函数
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (void)result;

    
    //缓冲接口回调
    result = (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
    (void)result;
    //第五步----------------------------------------
    // 设置播放状态
    result = (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    (void)result;
    //第六步----------------------------------------
    // 主动调用回调函数开始工作
    pcmBufferCallBack(pcmBufferQueue, this);
}


int HsAudio::resampleAudio(void **pcmbuf) {
    while(this->playstatus && !this->playstatus->exit){

        if (this->queue->getQueueSize() == 0){//没有数据
            if (!this->playstatus->load){
                this->playstatus->load = true;
                this->calljava->onCallLoading(this->playstatus->load,CHILD_THREAD);
            }
            continue;
        }else{
            if (this->playstatus->load){
                this->playstatus->load = false;
                this->calljava->onCallLoading(this->playstatus->load,CHILD_THREAD);
            }
        }


        AVPacket* avPacket = av_packet_alloc();
        if(this->queue->getPacket(avPacket)!=0){
            if (LOG_DEBUG){
                LOGE("queue->getPacket!=0");
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue ;
        }
        int ret = -1;
        ret = avcodec_send_packet(this->avCodecContext,avPacket);//送入编码器进行编码
        if (ret != 0 ){
            if (LOG_DEBUG){
                LOGE("avcodec_send_packet!=0");
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue ;
        }
        AVFrame* avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(this->avCodecContext,avFrame);//获取解码后的帧数据
        if (ret != 0 ){
            if (LOG_DEBUG){
                LOGE("avcodec_receive_frame!=0");
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue ;
        }

        //重采样
        if(avFrame->channels > 0 && avFrame->channel_layout == 0){
            //重新设置声道布局
            avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
        }else if (avFrame->channel_layout > 0 && avFrame->channels == 0){
            //重新设置声道数
            avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
        }


        SwrContext *swr_ctx = NULL;
//        swr_alloc_set_opts(struct SwrContext *s,
//        int64_t out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
//        int64_t  in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
//        int log_offset, void *log_ctx);
        swr_ctx = swr_alloc_set_opts(
                NULL,
                AV_CH_LAYOUT_STEREO,//立体声
                AV_SAMPLE_FMT_S16,//16位
                avFrame->sample_rate,
                avFrame->channel_layout,
                static_cast<AVSampleFormat>(avFrame->format),
                avFrame->sample_rate,
                NULL,
                NULL);

        if(!swr_ctx || swr_init(swr_ctx) <0){

            if (LOG_DEBUG){
                LOGE("swr_init(swr_ctx) == AVERROR");
            }

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            if(swr_ctx){
                swr_free(&swr_ctx);
            }
            continue ;
        }
//        swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
//        const uint8_t **in , int in_count);
        //重采样后得到采样后的数据个数，按理来说resample_nb == avFrame->nb_samples
        resample_nb = swr_convert(swr_ctx,  &resample_data,avFrame->nb_samples,
                             reinterpret_cast<const uint8_t **>(&avFrame->data), avFrame->nb_samples);


        *pcmbuf = resample_data;
        LOGI("resample_nb = %d,avFrame->nb_samples = %d",resample_nb,avFrame->nb_samples);

        int resample_data_size = resample_nb * avFrame->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
        //当前AVframe时间
        this->play_frame_now_time = avFrame->pts * av_q2d(this->FRAME_TIME_BASE);

        LOGI("重采样后的数据大小 %d,当前帧的时间 %ld",resample_data_size,play_frame_now_time);

        if(this->play_clock > this->play_frame_now_time){
            this->play_frame_now_time = this->play_clock;
        }
        this->play_clock = this->play_frame_now_time;
        this->play_clock += resample_data_size / (double)(this->sample_rate * 2 * 2);//1秒的采样
        if(this->play_clock - this->play_last_clock >= 0.5){//0.2秒回调一次
            this->play_last_clock = this->play_clock;//记录audio->play_clock的时间
            this->calljava->onCallTimeInfo(this->play_clock,this->total_duration,CHILD_THREAD);
        }

        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
        if(swr_ctx){
            swr_free(&swr_ctx);
        }
        return resample_data_size;
    }
    LOGI("重采样end");
    return 0;
}

SLuint32 HsAudio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate)
    {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate =  SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void HsAudio::resume() {
    if (pcmPlayerPlay){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void HsAudio::pause() {
    if (pcmPlayerPlay){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void HsAudio::stop() {
    if (pcmPlayerPlay){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void HsAudio::release() {
    stop();
    if (queue){
        delete queue;
        queue = NULL;
    }
    if(codecpar){
        codecpar = NULL;
    }
    if(avCodecContext != NULL)
    {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }
    if(pcmPlayerObject)
    {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }

    if(outputMixObject)
    {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if(engineObject)
    {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    if(playstatus)
    {
        playstatus = NULL;
    }
    if(calljava)
    {
        calljava = NULL;
    }
    this->play_clock = 0;
    this->play_last_clock = 0;
    this->total_duration = 0;
}

void HsAudio::setVolume(int volume) {
    if(volumeObject){
        //this->volume = volume;
        if(volume > 30)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -20);
        }
        else if(volume > 25)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -22);
        }
        else if(volume > 20)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -25);
        }
        else if(volume > 15)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -28);
        }
        else if(volume > 10)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -30);
        }
        else if(volume > 5)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -34);
        }
        else if(volume > 3)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -37);
        }
        else if(volume > 0)
        {
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -40);
        }
        else{
            (*volumeObject)->SetVolumeLevel(volumeObject, (100 - volume) * -100);
        }
    }
}

void HsAudio::setMute(int mute) {
    if(muteObject != NULL)
    {
        if(mute == 0)//right
        {
            (*muteObject)->SetChannelMute(muteObject, 1, false);
            (*muteObject)->SetChannelMute(muteObject, 0, true);
        }
        else if(mute == 1)//left
        {
            (*muteObject)->SetChannelMute(muteObject, 1, true);
            (*muteObject)->SetChannelMute(muteObject, 0, false);
        }
        else if(mute == 2)//center
        {
            (*muteObject)->SetChannelMute(muteObject, 1, false);
            (*muteObject)->SetChannelMute(muteObject, 0, false);
        }
    }
}

int HsAudio::getSoundTouchData() {
    int sound_num = 0;
    int buffer_size = 0;
    while (playstatus !=NULL && !playstatus->exit){
        sound_out_buffer = NULL;
        if(sound_finished){
            sound_finished = false;
            buffer_size = resampleAudio(reinterpret_cast<void **>(&sound_out_buffer));
            if (buffer_size > 0){//拿到重采样后的数据进行转换
                /*
                 * 因为FFmpeg解码出来的PCM数据是8bit （uint8）的，而SoundTouch中最低
                    是16bit（ 16bit integer samples ），所以我们需要将8bit的数据转换成16bit
                    后再给SoundTouch处理。处理方式：由于PCM数据在内存中是顺序排列的，所以我们先将第一个8bit的数据复制
                    到16bit内存的前8位，然后后8bit的数据再复制给16bit内存的后8bit，就能把16bit的内存填满，然后循环复制，直到把8bit的内存全部复制到16bit的内存
                    中，计算公式如下：
                    for(int i = 0; i < data_size / 2 + 1; i++)
                    {
	                  sampleBuffer[i] = (buffer[i * 2] | ((buffer[i * 2 + 1]) << 8));
                   }
                 * */
                for(int i = 0; i < buffer_size / 2 + 1; i++)
                {
                    sampleBuffer[i] = (sound_out_buffer[i * 2] | ((sound_out_buffer[i * 2 + 1]) << 8));
                }
                soundTouch->putSamples(sampleBuffer,resample_nb);
                sound_num = soundTouch->receiveSamples(sampleBuffer,buffer_size / (2 * 2));
            }else{
                soundTouch->flush();
            }
        }
        if (sound_num == 0){
            sound_finished = true;
            continue;
        }else{
            if (sound_out_buffer == NULL){
                sound_num = soundTouch->receiveSamples(sampleBuffer,buffer_size / (2 * 2));
                if (sound_num == 0){
                    sound_finished = true;
                    continue;
                }
            }
            return sound_num * 2 * 2;
        }
    }
    return 0;
}

void HsAudio::setPitch(float pitch) {
    if (soundTouch){
        soundTouch->setPitch(pitch);
    }
}

void HsAudio::setSpeed(float speed) {
    if (soundTouch){
        soundTouch->setTempo(speed);
    }
}

