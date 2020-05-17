//
// Created by haisheng on 2020/5/11.
//

#include "HsAudio.h"

HsAudio::HsAudio(HsPlaystatus* playstatus) {
    this->playstatus = playstatus;
    queue = new HsQueue(this->playstatus);
}

HsAudio::~HsAudio() {

}

void* decord_play(void* data){
    HsAudio* audio = static_cast<HsAudio *>(data);
    audio->resampleAudio();
    pthread_exit(&audio->thread_play);
}

void HsAudio::play() {
    pthread_create(&thread_play,NULL,decord_play,this);
}

int HsAudio::resampleAudio() {

}