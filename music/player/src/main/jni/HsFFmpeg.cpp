//
// Created by haisheng on 2020/5/11.
//

#include "HsFFmpeg.h"
#include <stdlib.h>

HsFFmpeg::HsFFmpeg(HsCalljava* calljava,HsPlaystatus* playstatus, const char* url) {
    this->calljava = calljava;
    this->playstatus = playstatus;
    this->url = static_cast<char *>(malloc(sizeof(char) * strlen(url)));
    strcpy(this->url,url);
    pthread_mutex_init(&decode_mutex,NULL);
    decode_exit = false;
}

HsFFmpeg::~HsFFmpeg() {
    free(url);
    pthread_mutex_destroy(&decode_mutex);
    if(pFormatContext){
        avformat_close_input(&pFormatContext);
        avformat_free_context(pFormatContext);
        pFormatContext =NULL;
    }
}


void* ffmpegthread(void* data){
    HsFFmpeg* hsFFmpeg = static_cast<HsFFmpeg *>(data);
    hsFFmpeg->decodeFFmpegThread();
    pthread_exit(&hsFFmpeg->decode_thread);
}

void HsFFmpeg::prepare() {
    pthread_create(&decode_thread,NULL,ffmpegthread,this);
}

//解码
void HsFFmpeg::start() {
    if (!this->audio){
        if(LOG_DEBUG){
            LOGE("audio is null");
            decode_exit = true;
            return;
        }
    }

    this->audio->play();


    int count = 0;
    while(playstatus != NULL && !playstatus->exit){
        AVPacket *avPacket = av_packet_alloc();

        //获取每一帧数据
        if (av_read_frame(this->pFormatContext,avPacket) == 0){
            if(avPacket->stream_index == this->audio->streamIndex){//音频数据
                count++;
                if(LOG_DEBUG)
                {
                    LOGD("解封装第 %d 帧", count);
                }
                this->audio->queue->putPacket(avPacket);
            }else{
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        }else{

            av_packet_free(&avPacket);
            av_free(avPacket);
            while (playstatus && !playstatus->exit){
                if (this->audio->queue->getQueueSize() > 0){
                    continue;
                }else{
                    playstatus->exit = true;
                    if(LOG_DEBUG)
                    {
                        LOGE("decode finished");
                    }
                    break;
                }
            }
            break;
        }
    }

    /*
    while(this->audio->queue->getQueueSize()>0){
        AVPacket* packet = av_packet_alloc();
        this->audio->queue->getPacket(packet);
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    */
    if(LOG_DEBUG)
    {
        LOGD("解码完成");
    }
    decode_exit = true;
}

int interruptCallback(void* ctx){
    HsFFmpeg* hsFFmpeg = static_cast<HsFFmpeg *>(ctx);
    if(hsFFmpeg->playstatus->exit){
        return AVERROR_EOF;
    }
    return 0;
}

//线程需要执行的函数
void HsFFmpeg::decodeFFmpegThread() {

    pthread_mutex_lock(&decode_mutex);

    if (LOG_DEBUG){
        LOGD("decodeFFmpegThread %s",this->url);
    }
    //1、注册解码器并初始化网络
    av_register_all();
    avformat_network_init();
    //2、打开文件或网络流
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    pFormatCtx->interrupt_callback.callback = interruptCallback;
    pFormatCtx->interrupt_callback.opaque = this;
    if (avformat_open_input(&pFormatCtx,this->url,NULL,NULL)!=0){
        if (LOG_DEBUG){
            LOGE("avformat_open_input error %s",this->url);
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    //3、获取流信息
    if (avformat_find_stream_info(pFormatCtx,NULL) < 0 ){
        if (LOG_DEBUG){
            LOGE("avformat_find_stream_info error %s",this->url);
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    this->pFormatContext = pFormatCtx;
    //4、获取音频流
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
         if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){//得到音频流
             if (!this->audio){
                 this->audio = new HsAudio(this->playstatus,this->calljava,pFormatCtx->streams[i]->codecpar,i);
                 this->audio->total_duration = pFormatCtx->duration / AV_TIME_BASE;
                 this->audio->FRAME_TIME_BASE = pFormatCtx->streams[i]->time_base;
             }
         }
    }
    //5、获取解码器
    AVCodec *dec = avcodec_find_decoder(this->audio->codecpar->codec_id);
    if(!dec){
        if (LOG_DEBUG){
            LOGE("获取不到解码器");
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    //6、利用解码器创建解码器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    if (!codecContext){
        if (LOG_DEBUG){
            LOGE("获取不到解码器上下文");
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    this->audio->avCodecContext = codecContext;
    if (avcodec_parameters_to_context(this->audio->avCodecContext,this->audio->codecpar) <0){
        if (LOG_DEBUG){
            LOGE("can not fill decodecctx");
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    //7、    打开解码器
    if (avcodec_open2(this->audio->avCodecContext,dec,0) <0){
        if(LOG_DEBUG)
        {
            LOGE("cant not open audio strames");
        }
        this->decode_exit = true;
        pthread_mutex_unlock(&decode_mutex);
        return;
    }
    //回调java方法
    this->calljava->onCallPrepare(CHILD_THREAD);
    pthread_mutex_unlock(&decode_mutex);
}

void HsFFmpeg::resume() {
    this->audio->resume();
}

void HsFFmpeg::pause() {
    this->audio->pause();
}

void HsFFmpeg::release() {
    if(playstatus->exit){
        if (LOG_DEBUG){
            LOGE("no playing");
        }
        return;
    }
    if (LOG_DEBUG){
        LOGD("开始释放ffmpeg");
    }
    playstatus->exit = true;

    pthread_mutex_lock(&decode_mutex);
    int sleepCount = 0;
    while(!decode_exit){//等待解码的线程退出
        if(sleepCount > 1000)
        {
            decode_exit = true;
        }else{
            if(LOG_DEBUG)
            {
                LOGE("wait ffmpeg  exit %d", sleepCount);
            }
            sleepCount++;
            av_usleep(1000 * 10);//暂停10毫秒
        }
    }
    if(LOG_DEBUG)
    {
        LOGE("释放 Audio");
    }
    if (audio){
        audio->release();
        delete audio;
        audio = NULL;
    }
    if(calljava){
        calljava = NULL;
    }
    if (playstatus){
        playstatus = NULL;
    }

}