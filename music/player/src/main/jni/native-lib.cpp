#include <jni.h>
#include <string>

#include "com_aispeech_player_Demo.h"
#include "AndroidLog.h"
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
