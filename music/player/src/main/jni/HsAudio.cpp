//
// Created by haisheng on 2020/5/11.
//

#include "HsAudio.h"

HsAudio::HsAudio(HsPlaystatus* playstatus) {
    this->playstatus = playstatus;
    queue = new HsQueue(this->playstatus);
    resample_data = static_cast<uint8_t *>(av_malloc(44100 * 2 * 2));//申请一秒的数据量大小
}

HsAudio::~HsAudio() {

}

void* decode_play(void* data){
    HsAudio* audio = static_cast<HsAudio *>(data);
    audio->resampleAudio();
    pthread_exit(&audio->thread_play);
}

void HsAudio::play() {
    pthread_create(&thread_play,NULL,decode_play,this);
}

int HsAudio::resampleAudio() {

    while(this->playstatus && !this->playstatus->exit){
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
        int resample_nb = swr_convert(swr_ctx,  &resample_data,avFrame->nb_samples,
                             reinterpret_cast<const uint8_t **>(&avFrame->data), avFrame->nb_samples);


        LOGI("resample_nb = %d,avFrame->nb_samples = %d",resample_nb,avFrame->nb_samples);

        resample_data_size = resample_nb * avFrame->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

        LOGI("重采样后的数据大小 %d",resample_data_size);

        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
        if(swr_ctx){
            swr_free(&swr_ctx);
        }
    }
    LOGI("重采样end");
    return 0;
}