package com.aispeech.player;

import android.text.TextUtils;
import android.util.Log;

import com.aispeech.listener.HsLoadingListener;
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

    HsLoadingListener mHsLoadListener;
    public void setHsLoadListener(HsLoadingListener hsLoadListener){
        this.mHsLoadListener = hsLoadListener;
    }


    public void prepare(){
        if (TextUtils.isEmpty(mSource)){
            Log.e(TAG,"prepare mSource == NULL");
            return;
        }
        onCallLoading(true);//正在加载
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepare(mSource);
            }
        }).start();
    }

    public void start(){
        if (TextUtils.isEmpty(mSource)){
            Log.e(TAG,"start mSource == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_start();
            }
        }).start();
    }

    public void pause(){
        if (TextUtils.isEmpty(mSource)){
            Log.e(TAG,"start pause == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_pause();
            }
        }).start();
    }

    public void resume(){
        if (TextUtils.isEmpty(mSource)){
            Log.e(TAG,"start resume == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_resume();
            }
        }).start();
    }


    /*
    *C++ 回调 java的方法
    *@author luhaisheng
    *@time 2020/5/11 12:15
    */
    private void onCallPrepare(){
        if (mHsPrepareListener!=null){
            mHsPrepareListener.prepare();
        }
    }

    /*
     *C++ 回调 java的方法
     *@author luhaisheng
     *@time 2020/5/11 12:15
     */
    private void onCallLoading(boolean load){
        if (mHsLoadListener!=null){
            mHsLoadListener.loading(load);
        }
    }


    private native void n_prepare(String source);
    private native void n_start();
    private native void n_resume();
    private native void n_pause();




}
