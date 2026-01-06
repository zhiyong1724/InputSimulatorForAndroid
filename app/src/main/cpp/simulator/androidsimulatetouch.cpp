#include "androidsimulatetouch.h"
extern JavaVM *gVM;
AndroidSimulateTouch::AndroidSimulateTouch()
{
    JNIEnv *env;
    if (gVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "GetEnv fail.\n");
        return;
    }
    jclass AndroidSimulateTouchClass = env->FindClass("com/chenzhiyong/inputsimulator/AndroidSimulateTouch");
    if (nullptr == AndroidSimulateTouchClass) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not found class.");
        return;
    }

    jmethodID constructorId = env->GetMethodID(AndroidSimulateTouchClass, "<init>", "()V");
    if (nullptr == constructorId) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not get method id.\n");
        return;
    }

    mClickId = env->GetMethodID(AndroidSimulateTouchClass, "click", "(III)V");
    if (nullptr == mClickId) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not get method id.\n");
        return;
    }

    mSwipeId = env->GetMethodID(AndroidSimulateTouchClass, "swipe", "(IIIII)V");
    if (nullptr == mClickId) {
        __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Not get method id.\n");
        return;
    }

    mJavaObject = env->NewGlobalRef(env->NewObject(AndroidSimulateTouchClass, constructorId));
}

AndroidSimulateTouch::~AndroidSimulateTouch()
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

void AndroidSimulateTouch::click(int x, int y, int duration)
{
    JNIEnv *env;
    bool isAttach = attachCurrentThread(&env);
    if (env != nullptr) {
        if (mJavaObject != nullptr) {
            env->CallVoidMethod(mJavaObject, mClickId, x, y, duration);
        }
        if (isAttach) {
            gVM->DetachCurrentThread();
        }
    }
}

void AndroidSimulateTouch::swipe(int x, int y, int endX, int endY, int duration)
{
    JNIEnv *env;
    bool isAttach = attachCurrentThread(&env);
    if (env != nullptr) {
        if (mJavaObject != nullptr) {
            env->CallVoidMethod(mJavaObject, mSwipeId , x, y, endX, endY, duration);
        }
        if (isAttach) {
            gVM->DetachCurrentThread();
        }
    }
}

bool AndroidSimulateTouch::attachCurrentThread(JNIEnv **env) {
    if (gVM->GetEnv(reinterpret_cast<void **>(env), JNI_VERSION_1_6) != JNI_OK) {
        if (gVM->AttachCurrentThread(env, nullptr) != JNI_OK) {
            __android_log_print(ANDROID_LOG_ERROR, "InputSimulator", "Attach current thread fail.\n");
            return false;
        }
        return true;
    }
    return false;
}
