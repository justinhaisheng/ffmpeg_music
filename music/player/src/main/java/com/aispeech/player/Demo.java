package com.aispeech.player;

/**
 * @创建者 luhaisheng
 * @创建时间 2020/5/6 23:17
 * @描述
 */
public class Demo {

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    public native String stringFromJNI();

}
