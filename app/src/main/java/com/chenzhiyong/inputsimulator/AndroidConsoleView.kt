package com.chenzhiyong.inputsimulator

import android.content.Intent

class AndroidConsoleView {
    fun log(text : String) {
        val intent = Intent(LOG_ACTION)
        intent.putExtra("text", text)
        InputSimulatorApplication.getInstance()?.sendBroadcast(intent)
    }
    companion object {
        val LOG_ACTION = "com.chenzhiyong.inputsimulator.log"
    }
}