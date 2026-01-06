#include <jni.h>
#include "androidconsoleview.h"
#include "androidsimulatetouch.h"
#include "simulator.h"
JavaVM *gVM = nullptr;
class InputSimulator
{
public:
    InputSimulator() : mSimulator(nullptr, nullptr, &mAndroidSimulateTouch, &mAndroidConsoleView)
    {
    }

    ~InputSimulator() {}

    void start()
    {
        mSimulator.start();
    }

    void stop()
    {
        mSimulator.stop();
    }

    bool loadScript(const std::string& script)
    {
        return mSimulator.loadScript(script);
    }
private:
    AndroidSimulateTouch mAndroidSimulateTouch;
    AndroidConsoleView mAndroidConsoleView;
    Simulator mSimulator;
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    gVM = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_chenzhiyong_inputsimulator_InputSimulator_newNativeObject(JNIEnv *env, jobject thiz) {
    auto inputSimulator = new InputSimulator();
    return (jlong)inputSimulator;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_chenzhiyong_inputsimulator_InputSimulator_deleteNativeObject(JNIEnv *env, jobject thiz,
                                                                      jlong obj) {
    InputSimulator *inputSimulator = (InputSimulator *)obj;
    if (inputSimulator != nullptr) {
        delete inputSimulator;
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_chenzhiyong_inputsimulator_InputSimulator_start(JNIEnv *env, jobject thiz, jlong obj) {
    InputSimulator *inputSimulator = (InputSimulator *)obj;
    if (inputSimulator != nullptr) {
        inputSimulator->start();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_chenzhiyong_inputsimulator_InputSimulator_stop(JNIEnv *env, jobject thiz, jlong obj) {
    InputSimulator *inputSimulator = (InputSimulator *)obj;
    if (inputSimulator != nullptr) {
        inputSimulator->stop();
    }
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_chenzhiyong_inputsimulator_InputSimulator_loadScript(JNIEnv *env, jobject thiz, jlong obj,
                                                              jstring script) {
    jboolean ret = false;
    InputSimulator *inputSimulator = (InputSimulator *)obj;
    if (inputSimulator != nullptr) {
        const char *pscript = env->GetStringUTFChars(script, nullptr);
        ret = inputSimulator->loadScript(pscript);
        env->ReleaseStringUTFChars(script, pscript);
    }
    return ret;
}