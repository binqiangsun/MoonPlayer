//
// Created by 孙斌强 on 2020-08-30.
//

#include "VideoChannel.h"

VideoChannel::VideoChannel(int stream_index, AVCodecContext *avCodecContext) :
BaseChannel(stream_index, avCodecContext){

}


void * task_video_decode(void * pVoid) {
    VideoChannel * videoChannel = static_cast<VideoChannel *>(pVoid);
    videoChannel->video_decode();
    return 0;
}

void * task_video_player(void * pVoid) {
    VideoChannel * videoChannel = static_cast<VideoChannel *>(pVoid);
    videoChannel->video_player();
    return 0;
}

//取出未解码队列数据
void VideoChannel::start() {
    //
    packetQueue.setFlag(1);


    //存放已经解码的数据
    frameQueue.setFlag(1);

    //解码的线程
    pthread_create(&pid_video_decode, 0, task_video_decode, this);

    // 播放的线程
    pthread_create(&pid_video_play, 0, task_video_player, this);

}

void VideoChannel::stop() {

}

void VideoChannel::video_decode() {
    AVPacket *packet;
    while (isPlaying) {
        LOGD("video-解码队列：循环获取解码的数据, 队列长度：%d", packetQueue.queueSize());
        // 生产快  消费慢
        // 消费速度比生成速度慢（生成100，只消费10个，这样队列会爆）
        // 内存泄漏点2，解决方案：控制队列大小
        if (isPlaying && frameQueue.queueSize() > 100) {
            // 休眠 等待队列中的数据被消费
            av_usleep(10 * 1000);
            continue;
        }

        int ret = this->packetQueue.pop(packet);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }

        ret = avcodec_send_packet(this->avCodecContext, packet);
        if (ret) {
            break;
        }

        AVFrame *avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(this->avCodecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            // 代表桢取的不完整
            continue;
        } else if (ret != 0) {
            //TODO 释放
            break;
        }

        //取到原始数据
        this->frameQueue.push(avFrame);
    }

    //TODO 出了循环，释放
}

void VideoChannel::video_player() {

    // yuv格式转化为 rgba， 显示到屏幕上
    SwsContext *swsContext = sws_getContext(this->avCodecContext->width, this->avCodecContext->height, this->avCodecContext->pix_fmt,
            this->avCodecContext->width, this->avCodecContext->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL);

    // 2.TODO 给dst_data rgba 这种 申请内存
    uint8_t * dst_data[4];
    int dst_linesize[4];
    AVFrame * frame = 0;

    av_image_alloc(dst_data, dst_linesize,
                   avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, 1);

    // 3.TODO 原始数据 格式转换的函数 （从队列中，拿到（原始）数据，一帧一帧的转换（rgba），一帧一帧的渲染到屏幕上）
    while(isPlaying) {
        LOGD("video-播放队列：循环队列中获取帧数据, 队列长度：%d", frameQueue.queueSize());
        int ret = frameQueue.pop(frame);

        // 如果停止播放，跳出循环, 出了循环，就要释放
        if (!isPlaying) {
            break;
        }

        if (!ret) {
            continue;
        }

        // 格式的转换 (yuv --> rgba)   frame->data == yuv是原始数据      dst_data是rgba格式的数据
        sws_scale(swsContext, frame->data,
                  frame->linesize, 0, avCodecContext->height, dst_data, dst_linesize);

        // 渲染，显示在屏幕上了
        // 渲染的两种方式：
        // 渲染一帧图像（宽，高，数据）
        renderCallback(dst_data[0], avCodecContext->width, avCodecContext->height , dst_linesize[0]);
        releaseAVFrame(&frame); // 渲染完了，frame没用了，释放掉
    }
    releaseAVFrame(&frame);
    isPlaying = 0;
    av_freep(&dst_data[0]);
    sws_freeContext(swsContext);

}


void VideoChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

VideoChannel::~VideoChannel() {

}

