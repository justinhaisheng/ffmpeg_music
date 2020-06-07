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
    jmid_db = env->GetMethodID(jclz,"onCallValumeDB","(I)V");
    jmid_pcm_to_aac = env -> GetMethodID(jclz,"encodecPcmToAAc","([BI)V");
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

void HsCalljava::onCallDB(int db, int thread_type) {
    if (!jmid_db){
        if (LOG_DEBUG){
            LOGE("jmid_db is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            LOGD("onCallDB MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_db,db);
    }else{
        if (LOG_DEBUG){
            LOGD("onCallDB CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_db,db);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallPcmToAAc(void* data, int size,int thread_type){
    if (!jmid_pcm_to_aac){
        if (LOG_DEBUG){
            LOGE("jmid_pcm_to_aac is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD){
        if (LOG_DEBUG){
            LOGD("onCallPcmToAAc MAIN_THREAD");
        }
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(data));
        this->jniEnv->CallVoidMethod(this->jobj,this->jmid_pcm_to_aac,jbuffer,size);
        jniEnv->DeleteLocalRef(jbuffer);
    }else{
        if (LOG_DEBUG){
            LOGD("onCallPcmToAAc CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2,0)!=JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jbyteArray jbuffer = jniEnv2->NewByteArray(size);
        jniEnv2->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(data));
        jniEnv2->CallVoidMethod(this->jobj,this->jmid_pcm_to_aac,jbuffer,size);
        jniEnv2->DeleteLocalRef(jbuffer);
        this->javaVm->DetachCurrentThread();
    }
}