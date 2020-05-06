package com.aispeech.music.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import com.aispeech.music.R;

import androidx.appcompat.app.AppCompatActivity;

public class MusicActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music);
    }

    public static void jumpMusicActivity(Context context){
        context.startActivity(new Intent(context,MusicActivity.class));
    }
}
