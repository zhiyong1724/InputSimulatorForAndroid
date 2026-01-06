package com.chenzhiyong.inputsimulator

import android.app.Application

class InputSimulatorApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        application = this
    }

    companion object {
        private var application : InputSimulatorApplication? = null
        fun getInstance() : InputSimulatorApplication? {
            return application
        }
    }
}