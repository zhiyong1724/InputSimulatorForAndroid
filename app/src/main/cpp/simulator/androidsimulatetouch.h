#ifndef __ANDROIDSIMULATEMTOUCH_H__
#define __ANDROIDSIMULATEMTOUCH_H__
#include "simulatetouch.h"
#include <jni.h>
#include <android/log.h>
class AndroidSimulateTouch : public SimulateTouch
{
public:
    AndroidSimulateTouch();
	virtual ~AndroidSimulateTouch();
    virtual void click(int x, int y, int duration) override;
    virtual void swipe(int x, int y, int endX, int endY, int duration) override;
private:
    bool attachCurrentThread(JNIEnv **env);
private:
    jobject mJavaObject;
    jmethodID mClickId;
    jmethodID mSwipeId;
};
#endif
