//
// Created by 孙斌强 on 2020-08-30.
//

#include "JNICallback.h"

// 可能主线程/异步线程
void JNICallback::onError(int thread_mode, int error_code) {

    if (thread_mode == THREAD_MAIN) {
        this->env->CallVoidMethod(this->instance, this->jmd_error);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *jniEnv = NULL;
        int ret = this->javaVm->AttachCurrentThread(&jniEnv, 0);
        if (ret != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(this->instance, this->jmd_error);

        //解除附加
        this->javaVm->DetachCurrentThread();
    }
}

// 可能主线程/异步线程
void JNICallback::onPrepared(int thread_mode) {

    if (thread_mode == THREAD_MAIN) {
        this->env->CallVoidMethod(this->instance, this->jmd_prepared);
    } else if (thread_mode == THREAD_CHILD) {
        JNIEnv *jniEnv = NULL;
        int ret = this->javaVm->AttachCurrentThread(&jniEnv, 0);
        if (ret != JNI_OK) {
            return;
        }
        jniEnv->CallVoidMethod(this->instance, this->jmd_prepared);

        //解除附加
        this->javaVm->DetachCurrentThread();
    }

}

JNICallback::JNICallback(JavaVM *javaVm, JNIEnv *env, jobject instance) {
    this->javaVm = javaVm;
    this->env = env;
    // 跨线程不能使用env

    // instance 有可能在Java层被释放
    this->instance = env->NewGlobalRef(instance); // 全局引用

    // 调用者就是MoonPlayer
    jclass moonPlayer = env->GetObjectClass(this->instance);
    const char *sig = "()V";
    this->jmd_prepared = env->GetMethodID(moonPlayer, "onPrepared", sig);

    const char *sig_error = "(I)V";
    this->jmd_error = env->GetMethodID(moonPlayer, "onError", sig_error);
}

JNICallback::~JNICallback() {
    this->javaVm = NULL;
    env->DeleteGlobalRef(this->instance);
    this->instance = NULL;
    env = NULL;
}
