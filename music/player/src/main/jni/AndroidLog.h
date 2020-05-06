//
// Created by haisheng.lu on 2019/12/21.
//

#ifndef MUSIC_TEST_ANDROIDLOG_H
#define MUSIC_TEST_ANDROIDLOG_H

#endif //MUSIC_TEST_ANDROIDLOG_H

#include <android/log.h>

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"haisheng",FORMAT,##__VA_ARGS__);
#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,"haisheng",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"haisheng",FORMAT,##__VA_ARGS__);
