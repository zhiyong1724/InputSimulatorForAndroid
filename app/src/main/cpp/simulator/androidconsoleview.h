//
// Created by tpp on 2026/1/5.
//

#ifndef __ANDROIDCONSOLEVIEW_H__
#define __ANDROIDCONSOLEVIEW_H__
#include "console.h"
#include <jni.h>
#include <android/log.h>
class AndroidConsoleView : public Console{
public:
    AndroidConsoleView();
    virtual ~AndroidConsoleView() override;
    virtual void log(const std::string &text) override;
private:
    bool attachCurrentThread(JNIEnv **env);
private:
    jobject mJavaObject;
    jmethodID mLogId;
};


#endif //__ANDROIDCONSOLEVIEW_H__
