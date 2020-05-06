package com.aispeech.music.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import com.aispeech.music.R;
import com.aispeech.player.Demo;

import androidx.appcompat.app.AppCompatActivity;

public class MusicActivity extends AppCompatActivity {
    Demo demo;
    TextView textView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music);
        demo = new Demo();
        textView = findViewById(R.id.sample_text);
        textView.setText(demo.stringFromJNI());
    }

    public static void jumpMusicActivity(Context context){
        context.startActivity(new Intent(context,MusicActivity.class));
    }
}
