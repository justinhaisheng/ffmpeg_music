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