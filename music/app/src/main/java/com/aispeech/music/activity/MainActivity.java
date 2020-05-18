package com.aispeech.music.activity;

import android.os.Bundle;
import android.util.Log;

import com.aispeech.music.R;
import com.hjq.permissions.OnPermission;
import com.hjq.permissions.XXPermissions;

import java.util.List;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    public static final String[] mpermissions = new String[]{
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE",
            "android.permission.RECORD_AUDIO",
            "android.permission.ACCESS_NETWORK_STATE",
            "android.permission.INTERNET",
            "android.permission.ACCESS_WIFI_STATE",
            "android.permission.MODIFY_AUDIO_SETTINGS"
    };

    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        hasPermissions();
    }

    private void hasPermissions() {
        Log.d(TAG,"hasPermissions()");
        if (XXPermissions.isHasPermission(MainActivity.this,mpermissions)) {
           MusicActivity.jumpMusicActivity(this);
           finish();
        }else {
            goPermissions();
        }
    }

    private void goPermissions() {
        Log.d(TAG,"goPermissions()");
        XXPermissions.with(this)
                .constantRequest()
                .permission(mpermissions)
                .request(new OnPermission() {
                    @Override
                    public void hasPermission(List<String> granted, boolean isAll) {
                        Log.d(TAG,"hasPermissions() isAll:"+isAll);
                        if (isAll){
                            MusicActivity.jumpMusicActivity(MainActivity.this);
                            finish();
                        }
                    }

                    @Override
                    public void noPermission(List<String> denied, boolean quick) {
                        Log.d(TAG,"noPermission() quick:"+quick);
                        if (quick)
                            XXPermissions.gotoPermissionSettings(MainActivity.this);
                        else
                            finish();
                    }
                });
    }
}
