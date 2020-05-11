//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSCALLJAVA_H
#define MUSIC_HSCALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include "AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class HsCalljava {
public:
    _JavaVM* javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj = NULL;

    jmethodID  jmid_prepare;

public:
    HsCalljava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~HsCalljava();

    void onCallPrepare(int thread_type);
};


#endif //MUSIC_HSCALLJAVA_H
