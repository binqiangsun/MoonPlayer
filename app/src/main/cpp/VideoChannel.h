//
// Created by 孙斌强 on 2020-08-30.
//

#ifndef MOONPLAYER_VIDEOCHANNEL_H
#define MOONPLAYER_VIDEOCHANNEL_H

#include "BaseChannel.h"
extern "C" {
};

typedef void (*RenderCallback) (uint8_t * , int , int, int);

class VideoChannel : public BaseChannel {

public:
    VideoChannel(int i, AVCodecContext *avCodecContext);

    ~VideoChannel();

    void start();

    void stop();

    void video_decode();

    void video_player();

    void setRenderCallback(RenderCallback renderCallback);


private:
    pthread_t pid_video_decode;
    pthread_t pid_video_play;
    RenderCallback  renderCallback;


};


#endif //MOONPLAYER_VIDEOCHANNEL_H
