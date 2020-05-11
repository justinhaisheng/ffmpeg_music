#include <jni.h>
#include <string>


#include "AndroidLog.h"
#include "HsFFmpeg.h"
#include "HsCalljava.h"

extern "C"
{
#include <libavformat/avformat.h>
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_aispeech_player_Demo_stringFromJNI
        (JNIEnv *env, jobject instance){

    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL)
    {
        switch (c_temp->type)
        {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s", c_temp->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", c_temp->name);
                break;
            default:
                LOGI("[Other]:%s", c_temp->name);
                break;
        }
        c_temp = c_temp->next;
    }
    std::string hello = "hello from jni";
    return env->NewStringUTF(hello.c_str());
}

HsFFmpeg* ffmpeg = NULL;
HsCalljava* calljava = NULL;
_JavaVM *javaVM = NULL;

extern "C"
JNIEXPORT void JNICALL Java_com_aispeech_player_HsPlay_n_1prepare
        (JNIEnv *env, jobject instance, jstring jsource){

    const char* source = env->GetStringUTFChars(jsource,0);
    if (!ffmpeg){
        if(!calljava){
            calljava = new HsCalljava(javaVM,env,&instance);
        }
        ffmpeg = new HsFFmpeg(calljava,source);
    }
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(jsource,source);
}

extern "C"
JNIEXPORT void JNICALL Java_com_aispeech_player_HsPlay_n_1start
        (JNIEnv *, jobject){

}


extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return result;
    }
    return JNI_VERSION_1_4;
}
