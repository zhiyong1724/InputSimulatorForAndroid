package com.chenzhiyong.inputsimulator

import android.content.Intent

class AndroidSimulateTouch {
    fun click(x : Int, y : Int, duration : Int) {
        val intent = Intent(InputAccessibilityService.CLICK_ACTION)
        intent.putExtra("x", x)
        intent.putExtra("y", y)
        intent.putExtra("duration", duration)
        InputSimulatorApplication.getInstance()?.sendBroadcast(intent)
    }

    fun swipe(x : Int, y : Int, endX : Int, endY : Int, duration : Int) {
        val intent = Intent(InputAccessibilityService.SWIPE_ACTION)
        intent.putExtra("x", x)
        intent.putExtra("y", y)
        intent.putExtra("endX", endX)
        intent.putExtra("endY", endY)
        intent.putExtra("duration", duration)
        InputSimulatorApplication.getInstance()?.sendBroadcast(intent)
    }
}