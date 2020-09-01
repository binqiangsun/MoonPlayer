//
// Created by 孙斌强 on 2020-08-30.
//

#ifndef MOONPLAYER_MOONPLAYER_H
#define MOONPLAYER_MOONPLAYER_H


#include <cstring>
#include <pthread.h>
#include "VideoChannel.h"
#include "AudioChannel.h"
#include "JNICallback.h"
#include <android/log.h>

extern "C" {
    #include <libavformat/avformat.h>
};

#define TAG "native-video" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__) // 定义LOGD类型


class MoonPlayer {

public:
    MoonPlayer();

    MoonPlayer(const char *string, JNICallback *jniCallback);

    virtual ~MoonPlayer();

    void prepare();

    void prepare_();

    void start();

    void start_();

    void setRenderCallback(RenderCallback renderCallback);


private:
    char *data_source = 0;
    pthread_t pid_prepare = 0;
    pthread_t pid_start;
    AVFormatContext *avFormatContext = 0;

    VideoChannel *videoChannel = NULL;
    AudioChannel *audioChannel = NULL;
    JNICallback *jniCallback = NULL;
    int isPlay = 0;

    RenderCallback  renderCallback;
};


#endif //MOONPLAYER_MOONPLAYER_H
