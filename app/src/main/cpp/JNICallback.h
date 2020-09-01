//
// Created by 孙斌强 on 2020-08-30.
//

#ifndef MOONPLAYER_JNICALLBACK_H
#define MOONPLAYER_JNICALLBACK_H

#include <jni.h>
#include "macro.h"


class JNICallback {

public:
    // 线程中不能操作env， 用JavaVM代替
    JNICallback(JavaVM *javaVm, JNIEnv *env, jobject instance);

    ~JNICallback();

    //回调方法 成功
    void onPrepared(int thread_mode); // thread_mode 区分线程：主线程/异步线程

    //失败回调
    void onError(int thread_mode, int error_code);


private:
    JavaVM *javaVm = NULL;
    JNIEnv *env = NULL;
    jobject instance;

    jmethodID  jmd_prepared;
    jmethodID  jmd_error;

};


#endif //MOONPLAYER_JNICALLBACK_H
