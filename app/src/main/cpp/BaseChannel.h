//
// Created by 孙斌强 on 2020-08-30.
//

#ifndef MOONPLAYER_BASECHANNEL_H
#define MOONPLAYER_BASECHANNEL_H


#include "safe_queue.h"
#include <android/log.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
};

#define TAG "native-video" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__) // 定义LOGD类型

class BaseChannel {

public:
    int stream_index;
    AVCodecContext *avCodecContext;

    BaseChannel(int stream_index, AVCodecContext *avCodecContext) {
        this->stream_index = stream_index;
        this->avCodecContext = avCodecContext;
        packetQueue.setReleaseCallback(releaseAVPacket);
        frameQueue.setReleaseCallback(releaseAVFrame);
    }

    ~BaseChannel() {

        packetQueue.clearQueue();
        frameQueue.clearQueue();
    }

    // 音频： aac， 视频：h264
    SafeQueue<AVPacket *> packetQueue;

    // 音频： pcm,  视频：
    SafeQueue<AVFrame *> frameQueue;

    bool isPlaying = 1;

    /**
     * 释放AVPacket 队列
     * @param avPacket
     */
    static void releaseAVPacket(AVPacket ** avPacket) {
        if (avPacket) {
            av_packet_free(avPacket);
            *avPacket = 0;
        }
    }

    /**
     * 释放AVFrame 队列
     * @param avFrame
     */
    static void releaseAVFrame(AVFrame ** avFrame) {
        if (avFrame) {
            av_frame_free(avFrame);
            *avFrame = 0;
        }
    }

};


#endif //MOONPLAYER_BASECHANNEL_H
