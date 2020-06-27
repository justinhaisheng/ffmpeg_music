//
// Created by haisheng.lu on 2020/6/27.
//

#include "HsPcmBean.h"

HsPcmBean::HsPcmBean(SAMPLETYPE *buffer, int size) {
    this->buffer = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * size));
    this->buffsize = size;
    memcpy(this->buffer,buffer,size);
}

HsPcmBean::~HsPcmBean() {
    free(this->buffer);
    this->buffer = NULL;
}
