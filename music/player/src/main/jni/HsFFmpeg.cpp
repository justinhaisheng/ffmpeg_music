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
}

HsFFmpeg::~HsFFmpeg() {

}


void* ffmpegthread(void* data){
    HsFFmpeg* hsFFmpeg = static_cast<HsFFmpeg *>(data);
    hsFFmpeg->decodeFFmpegThread();
    pthread_exit(&hsFFmpeg->decodeThread);
}

void HsFFmpeg::prepare() {
    pthread_create(&decodeThread,NULL,ffmpegthread,this);
}

//解码
void HsFFmpeg::start() {
    if (!this->audio){
        if(LOG_DEBUG){
            LOGE("audio is null");
            return;
        }
    }

    this->audio->play();


    int count = 0;
    while(1){
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
}

//线程需要执行的函数
void HsFFmpeg::decodeFFmpegThread() {
    if (LOG_DEBUG){
        LOGD("decodeFFmpegThread %s",this->url);
    }
    //1、注册解码器并初始化网络
    av_register_all();
    avformat_network_init();
    //2、打开文件或网络流
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx,this->url,NULL,NULL)!=0){
        if (LOG_DEBUG){
            LOGE("avformat_open_input error %s",this->url);
        }
        return;
    }
    //3、获取流信息
    if (avformat_find_stream_info(pFormatCtx,NULL) < 0 ){
        if (LOG_DEBUG){
            LOGE("avformat_find_stream_info error %s",this->url);
        }
        return;
    }
    this->pFormatContext = pFormatCtx;
    //4、获取音频流
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
         if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){//得到音频流
             if (!this->audio){
                 this->audio = new HsAudio(this->playstatus);
                 //获取音频的流id和参数
                 this->audio->streamIndex = i;
                 this->audio->codecpar = pFormatCtx->streams[i]->codecpar;
             }
         }
    }
    //5、获取解码器
    AVCodec *dec = avcodec_find_decoder(this->audio->codecpar->codec_id);
    if(!dec){
        if (LOG_DEBUG){
            LOGE("获取不到解码器");
        }
        return;
    }
    //6、利用解码器创建解码器上下文
    AVCodecContext *codecContext = avcodec_alloc_context3(dec);
    if (!codecContext){
        if (LOG_DEBUG){
            LOGE("获取不到解码器上下文");
        }
        return;
    }
    this->audio->avCodecContext = codecContext;
    if (avcodec_parameters_to_context(this->audio->avCodecContext,this->audio->codecpar) <0){
        if (LOG_DEBUG){
            LOGE("can not fill decodecctx");
        }
        return;
    }
    //7、    打开解码器
    if (avcodec_open2(this->audio->avCodecContext,dec,0) <0){
        if(LOG_DEBUG)
        {
            LOGE("cant not open audio strames");
        }
        return;
    }
    //回调java方法
    this->calljava->onCallPrepare(CHILD_THREAD);
}