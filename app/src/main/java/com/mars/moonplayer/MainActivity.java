package com.mars.moonplayer;

import android.os.Bundle;
import android.view.SurfaceView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {


    private SurfaceView surfaceView;
    private final String videoPath = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
    private MoonPlayer moonPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = findViewById(R.id.surface_view);

        moonPlayer = new MoonPlayer();
        moonPlayer.setDataSource(videoPath);
        moonPlayer.setSurfaceView(surfaceView);
        moonPlayer.setOnPreparedListener(new MoonPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "准备好了，可以开始播放...", Toast.LENGTH_LONG).show();
                    }
                });
                moonPlayer.start();
            }

            @Override
            public void onError(final String errorText) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, errorText, Toast.LENGTH_LONG).show();
                    }
                });
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        moonPlayer.prepare();
    }

    @Override
    protected void onPause() {
        super.onPause();
        moonPlayer.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        moonPlayer.release();
    }
}
