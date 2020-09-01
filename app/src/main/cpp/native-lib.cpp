#include <jni.h>
#include <string>
#include "MoonPlayer.h"
#include "JNICallback.h"
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的


JavaVM *javaVm = NULL;
MoonPlayer *moonPlayer = NULL;
ANativeWindow *nativeWindow = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 静态初始化 互斥锁


int JNI_OnLoad(JavaVM *javaVm, void *pVoid) {
    ::javaVm = javaVm;
    return JNI_VERSION_1_6;
}


/**
 * 专门渲染的函数
 * @param src_data 解码后的视频 rgba 数据
 * @param width 宽信息
 * @param height 高信息
 * @param src_liinesize 行数size相关信息
 */
void renderFrame(uint8_t * src_data, int width, int height, int src_liinesize) {
    pthread_mutex_lock(&mutex);

    if (!nativeWindow) {
        pthread_mutex_unlock(&mutex);
    }

    // 设置窗口属性
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height , WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(nativeWindow, &windowBuffer, 0)) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }

    // 填数据到buffer，其实就是修改数据
    uint8_t * dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int lineSize = windowBuffer.stride * 4; // RGBA
    // 下面就是逐行Copy了
    for (int i = 0; i < windowBuffer.height; ++i) {
        // 一行Copy
        memcpy(dst_data + i * lineSize, src_data + i * src_liinesize, lineSize);
    }

    ANativeWindow_unlockAndPost(nativeWindow);
    pthread_mutex_unlock(&mutex);
}


extern "C" JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_prepareNative(JNIEnv *env, jobject thiz, jstring data_source) {

    JNICallback *jniCallback = new JNICallback(::javaVm, env, thiz);
    const char *dataSource = env->GetStringUTFChars(data_source, NULL);
    moonPlayer = new MoonPlayer(dataSource, jniCallback);
    moonPlayer->setRenderCallback(renderFrame);
    moonPlayer->prepare();

    env->ReleaseStringUTFChars(data_source, dataSource);

}

extern "C" JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_startNative(JNIEnv *env, jobject thiz) {
    if (moonPlayer != NULL) {
        moonPlayer->start();
    }

}


extern "C" JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_stopNative(JNIEnv *env, jobject thiz) {
    // TODO: implement stopNative()
}extern "C"
JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_releaseNative(JNIEnv *env, jobject thiz) {
    // TODO: implement releaseNative()
}extern "C"
JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_setSurfaceNative(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex);
    if (nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
    }
    // 创建新的窗口用于视频显示
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}extern "C"
JNIEXPORT void JNICALL
Java_com_mars_moonplayer_MoonPlayer_getFFmpegVersion(JNIEnv *env, jobject thiz) {
    // TODO: implement getFFmpegVersion()
}