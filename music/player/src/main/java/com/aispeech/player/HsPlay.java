package com.aispeech.player;

import android.text.TextUtils;
import android.util.Log;

import com.aispeech.listener.HsPrepareListener;

/**
 * @创建者 luhaisheng
 * @创建时间 2020/5/11 11:49
 * @描述
 */
public class HsPlay {

    public final static String TAG = "HsPlay";

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

    private String mSource;//数据源

    public void setSource(String source){
        this.mSource = source;
    }

    HsPrepareListener mHsPrepareListener;
    public void setHsPrepareListener(HsPrepareListener hsPrepareListener){
        this.mHsPrepareListener = hsPrepareListener;
    }

    public void prepare(){
        if (TextUtils.isEmpty(mSource)){
            Log.d(TAG,"prepare");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepare(mSource);
            }
        }).start();
    }

    /*
    *C++ 回调 java的方法
    *@author luhaisheng
    *@time 2020/5/11 12:15
    */
    public void onCallPrepare(){
        if (mHsPrepareListener!=null){
            mHsPrepareListener.prepare();
        }
    }
    public native void n_prepare(String source);
    public native void n_start();





}
