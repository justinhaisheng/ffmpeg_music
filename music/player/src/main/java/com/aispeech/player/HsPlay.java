package com.aispeech.player;

import android.text.TextUtils;
import android.util.Log;

import com.aispeech.HsTimeInfoBean;
import com.aispeech.listener.HsCompleteListener;
import com.aispeech.listener.HsErrorListener;
import com.aispeech.listener.HsLoadingListener;
import com.aispeech.listener.HsOnTimeInfoListener;
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
    private boolean mNextSource = false;//下一个数据源
    public void setSource(String source){
        this.mSource = source;
    }

    public void setNextSource(String nextSource){
        if (!TextUtils.isEmpty(nextSource)){
            this.mSource = nextSource;
            mNextSource = true;
        }else{
            mNextSource = false;
        }
    }

    public boolean getNextSource(){
        return mNextSource;
    }


    HsPrepareListener mHsPrepareListener;
    public void setHsPrepareListener(HsPrepareListener hsPrepareListener){
        this.mHsPrepareListener = hsPrepareListener;
    }

    HsLoadingListener mHsLoadListener;
    public void setHsLoadListener(HsLoadingListener hsLoadListener){
        this.mHsLoadListener = hsLoadListener;
    }

    HsOnTimeInfoListener mHsOnTimeInfoListener;
    public void setHsOnTimeInfoListener(HsOnTimeInfoListener hsOnTimeInfoListener){
        this.mHsOnTimeInfoListener = hsOnTimeInfoListener;
    }

    HsErrorListener mHsErrorListener;
    public void setHsErrorListener(HsErrorListener hsErrorListener){
        this.mHsErrorListener = hsErrorListener;
    }

    HsCompleteListener mHsCompleteListener;
    public void setHsCompleteListener(HsCompleteListener hsCompleteListener){
        this.mHsCompleteListener = hsCompleteListener;
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

    public void stop(){
        onCallTimeInfo(0,0);
        hsTimeInfoBean = null;
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_stop();
            }
        }).start();
    }

    public void seek(final int seconds){
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_seek(seconds);
            }
        }).start();
    }

    public int getDuration(){
        if(hsTimeInfoBean!=null){
            return hsTimeInfoBean.getTotalTime();
        }
        return 0;
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

    private static HsTimeInfoBean hsTimeInfoBean;
    private void onCallTimeInfo(int currentTime, int totalTime)
    {
        if(mHsOnTimeInfoListener != null)
        {
            if(hsTimeInfoBean == null)
            {
                hsTimeInfoBean = new HsTimeInfoBean();
            }
            hsTimeInfoBean.setCurrentTime(currentTime);
            hsTimeInfoBean.setTotalTime(totalTime);
            mHsOnTimeInfoListener.onTimeInfo(hsTimeInfoBean);
        }
    }

    private void onCallError(int code,String codemsg){
        stop();
        if (mHsErrorListener!=null){
            mHsErrorListener.error(code,codemsg);
        }
    }

    private void onCallComplete(){
        stop();
        if (mHsCompleteListener!=null){
            mHsCompleteListener.complete();
        }
    }


    private native void n_prepare(String source);
    private native void n_start();
    private native void n_resume();
    private native void n_pause();
    private native void n_stop();
    private native void n_seek(int sencods);


}
