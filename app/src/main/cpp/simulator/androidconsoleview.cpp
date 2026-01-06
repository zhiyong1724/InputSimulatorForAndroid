//
// Created by tpp on 2026/1/5.
//

#include "androidconsoleview.h"
extern JavaVM *gVM;
AndroidConsoleView::AndroidConsoleView()
{
    JNIEnv *env;
    if (gVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "GetEnv fail.\n");
        return;
    }
    jclass androidConsoleViewClass = env->FindClass("com/chenzhiyong/inputsimulator/AndroidConsoleView");
    if (nullptr == androidConsoleViewClass) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not found class.");
        return;
    }
    jmethodID constructorId = env->GetMethodID(androidConsoleViewClass, "<init>", "()V");
    if (nullptr == constructorId) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not get method id.\n");
        return;
    }

    mLogId = env->GetMethodID(androidConsoleViewClass, "log", "(Ljava/lang/String;)V");
    if (nullptr == mLogId) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not get method id.\n");
        return;
    }

    mJavaObject = env->NewGlobalRef(env->NewObject(androidConsoleViewClass, constructorId));
}

AndroidConsoleView::~AndroidConsoleView()
{
    JNIEnv *env;
    if (gVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "GetEnv fail.\n");
        return;
    }
    if (mJavaObject != nullptr) {
        env->DeleteGlobalRef(mJavaObject);
    }
}

void AndroidConsoleView::log(const std::string &text)
{
    JNIEnv *env;
    bool isAttach = attachCurrentThread(&env);
    if (env != nullptr) {
        if (mJavaObject != nullptr) {
            jstring jtext = env->NewStringUTF(text.c_str());
            env->CallVoidMethod(mJavaObject, mLogId, jtext);
        }
        if (isAttach) {
            gVM->DetachCurrentThread();
        }
    }
}

bool AndroidConsoleView::attachCurrentThread(JNIEnv **env) {
    if (gVM->GetEnv(reinterpret_cast<void **>(env), JNI_VERSION_1_6) != JNI_OK) {
        if (gVM->AttachCurrentThread(env, nullptr) != JNI_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Attach current thread fail.\n");
            return false;
        }
        return true;
    }
    return false;
}