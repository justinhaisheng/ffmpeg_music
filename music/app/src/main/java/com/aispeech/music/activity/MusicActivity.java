package com.aispeech.music.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.aispeech.HsTimeInfoBean;
import com.aispeech.listener.HsLoadingListener;
import com.aispeech.listener.HsOnTimeInfoListener;
import com.aispeech.listener.HsPrepareListener;
import com.aispeech.music.R;
import com.aispeech.player.HsPlay;

import androidx.appcompat.app.AppCompatActivity;

public class MusicActivity extends AppCompatActivity {

    HsPlay mHsPlay;
    private TextView tvTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music);
        tvTime = findViewById(R.id.tv_time);
        mHsPlay = new HsPlay();
        mHsPlay.setHsPrepareListener(new HsPrepareListener() {
            @Override
            public void prepare() {
                Log.d(HsPlay.TAG,"prepare");
                mHsPlay.start();
            }
        });
        mHsPlay.setHsLoadListener(new HsLoadingListener() {
            @Override
            public void loading(boolean load) {
                if (load){
                    Log.d(HsPlay.TAG,"加载中...");
                }else{
                    Log.d(HsPlay.TAG,"播放中...");
                }
            }
        });
        mHsPlay.setHsOnTimeInfoListener(new HsOnTimeInfoListener() {
            @Override
            public void onTimeInfo(HsTimeInfoBean timeInfoBean) {
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                handler.sendMessage(message);
                Log.d(HsPlay.TAG,"当前时间："+timeInfoBean.getCurrentTime()+" 总时间："+timeInfoBean.getTotalTime());
            }
        });
        //http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3
        mHsPlay.setSource("/sdcard/1.mp3");
    }

    public static void jumpMusicActivity(Context context){
        context.startActivity(new Intent(context,MusicActivity.class));
    }

    public void begin(View view) {
        mHsPlay.prepare();
    }

    public void resume(View view){
        mHsPlay.resume();
    }

    public void pause(View view){
        mHsPlay.pause();
    }

    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what == 1)
            {
                HsTimeInfoBean wlTimeInfoBean = (HsTimeInfoBean) msg.obj;
                tvTime.setText(HsTimeUtil.secdsToDateFormat(wlTimeInfoBean.getTotalTime(), wlTimeInfoBean.getTotalTime())
                        + "/" + HsTimeUtil.secdsToDateFormat(wlTimeInfoBean.getCurrentTime(), wlTimeInfoBean.getTotalTime()));
            }
        }
    };
}
