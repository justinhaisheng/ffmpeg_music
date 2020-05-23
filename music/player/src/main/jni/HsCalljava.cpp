//
// Created by haisheng on 2020/5/11.
//

#include "HsCalljava.h"

HsCalljava::HsCalljava(_JavaVM *javaVM, JNIEnv *env, jobject obj) {
    this->javaVm = javaVM;
    this->jniEnv = env;
    this->jobj = env->NewGlobalRef(obj);

    jclass jclz = env->GetObjectClass(obj);
    if (!jclz){
        if (LOG_DEBUG){
            LOGE("get jclass wrong");
        }
        return ;
    }
    jmid_prepare = env->GetMethodID(jclz,"onCallPrepare","()V");
    jmid_load = env->GetMethodID(jclz,"onCallLoading","(Z)V");
    jmid_timeinfo = env->GetMethodID(jclz,"onCallTimeInfo","(II)V");
    jmid_error = env->GetMethodID(jclz,"onCallError","(ILjava/lang/String;)V");

    jmid_complete = env->GetMethodID(jclz,"onCallComplete","()V");
}

HsCalljava::~HsCalljava() {

}

void HsCalljava::onCallPrepare(int thread_type) {
    if (!jmid_prepare){
        if (LOG_DEBUG){
            LOGE("jmid_prepare is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_prepare);
    }else{
        if (LOG_DEBUG){
            LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_prepare);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallLoading(bool load,int thread_type) {
    if (!jmid_load){
        if (LOG_DEBUG){
            LOGE("jmid_load is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
           // LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_load,load);
    }else{
        if (LOG_DEBUG){
          //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_load,load);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallTimeInfo(int current, int total, int thread_type) {
    if (!jmid_timeinfo){
        if (LOG_DEBUG){
            LOGE("jmid_timeinfo is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            // LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_timeinfo,current,total);
    }else{
        if (LOG_DEBUG){
            //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_timeinfo,current,total);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallError(int code, char *msg, int thread_type) {
    if (!jmid_error){
        if (LOG_DEBUG){
            LOGE("jmid_error is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            // LOGD("onCallPrepare MAIN_THREAD");
        }
        jstring jmsg = jniEnv->NewStringUTF(msg);
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_error,code,jmsg);
        this->jniEnv->DeleteLocalRef(jmsg);
    }else{
        if (LOG_DEBUG){
            //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jstring jmsg = jniEnv2->NewStringUTF(msg);
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_error,code,jmsg);
        jniEnv2->DeleteLocalRef(jmsg);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallComplete(int thread_type) {
    if (!jmid_complete){
        if (LOG_DEBUG){
            LOGE("jmid_complete is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            LOGD("onCallComplete MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_prepare);
    }else{
        if (LOG_DEBUG){
            LOGD("onCallComplete CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_complete);
        this->javaVm->DetachCurrentThread();
    }
}