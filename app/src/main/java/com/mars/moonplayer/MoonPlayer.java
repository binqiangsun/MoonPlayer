package com.mars.moonplayer;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MoonPlayer implements SurfaceHolder.Callback, IPlayer{

    private String dataSource;
    private SurfaceHolder surfaceHolder;
    private OnPreparedListener onPreparedListener;

    static {
        System.loadLibrary("player-lib");
    }

    public MoonPlayer() {

    }

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        //每次
        if (null != surfaceHolder) {
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        setSurfaceNative(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void prepare() {
        prepareNative(this.dataSource);

    }

    @Override
    public void start() {
        startNative();
    }

    @Override
    public void stop() {
        stopNative();
    }

    @Override
    public void release() {
        releaseNative();
    }


    /**
     * native区域
     */

    public native void prepareNative(String dataSource);
    public native void startNative();
    public native void stopNative();
    public native void releaseNative();
    public native void setSurfaceNative(Surface surface);
    public native void getFFmpegVersion();


    /**
     * 给native调用
     */
    public void onPrepared() {
        if (onPreparedListener != null) {
            onPreparedListener.onPrepared();
        }
    }

    public void onError(int errorCode) {
        if (onPreparedListener != null) {
            String errorText = "错误:" + errorCode;
            onPreparedListener.onError(errorText);
        }
    }

    // 通知Activity
    interface OnPreparedListener {
        void onPrepared();
        void onError(String errorText);
    }

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

}
