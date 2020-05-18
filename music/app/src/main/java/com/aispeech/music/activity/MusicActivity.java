package com.aispeech.music.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.aispeech.listener.HsLoadingListener;
import com.aispeech.listener.HsPrepareListener;
import com.aispeech.music.R;
import com.aispeech.player.HsPlay;

import androidx.appcompat.app.AppCompatActivity;

public class MusicActivity extends AppCompatActivity {

    HsPlay mHsPlay;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music);
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


}
