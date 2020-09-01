//
// Created by 孙斌强 on 2020-08-30.
//

#include "MoonPlayer.h"
#include "JNICallback.h"

//
void * customTaskPrepareThread(void *pVoid) {
    MoonPlayer *moonPlayer = static_cast<MoonPlayer *>(pVoid);
    moonPlayer->prepare_();
    return 0;
}

void * customTaskStartThread(void *pVoid) {
    MoonPlayer *moonPlayer = static_cast<MoonPlayer *>(pVoid);
    moonPlayer->start_();
    return 0;
}

MoonPlayer::MoonPlayer() {}

MoonPlayer::~MoonPlayer() {
    if (this->data_source) {
        delete this->data_source;
        this->data_source = 0;
    }

}

MoonPlayer::MoonPlayer(const char *data_source, JNICallback *jniCallback) {
    // c: aaa c++: aaa+\r
    this->data_source = new char[strlen(data_source)+1];
    strcpy(this->data_source, data_source);
    this->jniCallback = jniCallback;
}

// 解码，音频流，视频流， 字母
void MoonPlayer::prepare() {
    pthread_create(&this->pid_prepare, 0, customTaskPrepareThread, this);
}

void MoonPlayer::prepare_() {
    //拆
    this->avFormatContext = avformat_alloc_context();

    //字典，打开方式
    AVDictionary *dictionary = 0;
    av_dict_set(&dictionary, "timeout", "5000000", 0); // 单位是微妙

    //检查是否被损坏
    int ret = avformat_open_input(&this->avFormatContext, this->data_source, NULL, &dictionary);
    av_dict_free(&dictionary); // 释放字典
    if (ret < 0) {
        this->jniCallback->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }

    //寻找音频，视频流
    ret = avformat_find_stream_info(this->avFormatContext, NULL);
    if (ret < 0) {
        this->jniCallback->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        return;
    }

    // 循环遍历
    for (int i = 0; i < avFormatContext->nb_streams; i ++) {
        AVStream *stream = avFormatContext->streams[i];

        //得到解码器ID
        AVCodecParameters *codecParameters = stream->codecpar;
        //解码器
        AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
        if (!codec) {
            this->jniCallback->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        // 解码器上下文
        AVCodecContext *avCodecContext = avcodec_alloc_context3(codec);
        if (!avCodecContext) {
            this->jniCallback->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }
        //解码器上下文设置
        ret = avcodec_parameters_to_context(avCodecContext, codecParameters);
        if (ret < 0) {
            this->jniCallback->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        // 打开解码器
        ret = avcodec_open2(avCodecContext, codec, NULL);
        if (ret < 0) {
            this->jniCallback->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            return;
        }

        // 解码器打开成功
        // 区分音频 / 视频
        if (AVMEDIA_TYPE_AUDIO == codecParameters->codec_type) {
            this->audioChannel = new AudioChannel(i, avCodecContext);
            LOGD("成功创建音频通道: %d", i);
        } else if (AVMEDIA_TYPE_VIDEO == codecParameters->codec_type) {
            this->videoChannel = new VideoChannel(i, avCodecContext);
            videoChannel->setRenderCallback(renderCallback);
            LOGD("成功创建视频通道: %d", i);
        } else {

        }
    }

    // TODO 第十一步：如果流中 没有音频 也 没有视频
    if (!audioChannel || !videoChannel) {
        // 把错误信息，告诉给Java层去（回调给Java）
        jniCallback->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
        return;
    }


    //回调Java，执行完毕
    this->jniCallback->onPrepared(THREAD_CHILD);
}

//
void MoonPlayer::start() {
    // 让两个通道运行起来

    isPlay = 9;

    if (videoChannel) {
        this->videoChannel->start();
    }
    if (audioChannel) {
        audioChannel->start();
    }

    pthread_create(&pid_start, 0, customTaskStartThread, this);

}

//线程执行
void MoonPlayer::start_() {

    // 读取视频包
    while (this->isPlay) {
        LOGD("开始读取视频包数据, 视频通道: %d, 音频通道:%d", videoChannel->stream_index, audioChannel->stream_index);
        //未解码的
        AVPacket *packet = av_packet_alloc();
        int ret = av_read_frame(this->avFormatContext, packet);
        //
        if (ret == 0) {
            if (videoChannel && videoChannel->stream_index == packet->stream_index) {
                LOGD("读取到视频数据");
                //视频包
                videoChannel->packetQueue.push(packet);
                LOGD("视频队列长度：%d", videoChannel->packetQueue.queueSize());
            } else if (audioChannel && audioChannel->stream_index == packet->stream_index) {
                // 如果他们两 相等 说明是音频  音频包
                // 未解码的 音频数据包 加入到队列
                LOGD("读取到音频数据");
                audioChannel->packetQueue.push(packet);
                LOGD("音频队列长度：%d", audioChannel->packetQueue.queueSize());
            } else {
                LOGD("读取到其他数据: %d", packet->stream_index);
            }
        } else if (ret == AVERROR_EOF) {
            //TODO
        } else {
            break;
        }
    }

    //释放
    isPlay = 0;
    videoChannel->stop();
    audioChannel->stop();

}

void MoonPlayer::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}
