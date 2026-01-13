package com.chenzhiyong.inputsimulator

import android.accessibilityservice.AccessibilityService
import android.accessibilityservice.GestureDescription
import android.accessibilityservice.GestureDescription.StrokeDescription
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Path
import android.os.Build
import android.view.accessibility.AccessibilityEvent
import androidx.annotation.RequiresApi


class InputAccessibilityService : AccessibilityService() {
    private var inputReceiver: InputReceiver? = null
    private var inputSimulator: InputSimulator? = null

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onServiceConnected() {
        super.onServiceConnected()
        inputSimulator = InputSimulator()
        // 注册广播接收器
        inputReceiver = InputReceiver()
        val filter = IntentFilter()
        filter.addAction(START_ACTION)
        filter.addAction(STOP_ACTION)
        filter.addAction(LOAD_SCRIPT_ACTION)
        filter.addAction(CLICK_ACTION)
        filter.addAction(SWIPE_ACTION)
        registerReceiver(inputReceiver, filter, RECEIVER_EXPORTED)
    }

    override fun onAccessibilityEvent(p0: AccessibilityEvent?) {
    }

    override fun onInterrupt() {
    }

    override fun onDestroy() {
        super.onDestroy()
        if (inputReceiver != null) {
            unregisterReceiver(inputReceiver)
        }
        inputSimulator?.releaseNativeObject()
    }

    // 模拟点击方法
    private fun simulateClick(x: Float, y: Float, duration: Long) {
        val clickPath: Path = Path()
        clickPath.moveTo(x, y)

        val builder = GestureDescription.Builder()
        val gestureDescription = builder
            .addStroke(StrokeDescription(clickPath, 0, duration))
            .build()

        dispatchGesture(gestureDescription, null, null)
    }

    // 模拟滑动方法
    private fun simulateSwipe(startX: Float, startY: Float, endX: Float, endY: Float, duration: Long) {
        val swipePath: Path = Path()
        swipePath.moveTo(startX, startY)
        swipePath.lineTo(endX, endY)

        val builder = GestureDescription.Builder()
        val gestureDescription = builder
            .addStroke(StrokeDescription(swipePath, 0, duration))
            .build()

        dispatchGesture(gestureDescription, null, null)
    }

    // 广播接收器类
    private inner class InputReceiver : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when(intent.action) {
                START_ACTION -> {
                    inputSimulator?.start()
                }
                STOP_ACTION -> {
                    inputSimulator?.stop()
                    stopSelf()
                }
                LOAD_SCRIPT_ACTION -> {
                    val script = intent.getStringExtra("script")
                    script?.let { inputSimulator?.loadScript(it) }
                }
                CLICK_ACTION -> {
                    val x = intent.getIntExtra("x", 0)
                    val y = intent.getIntExtra("y", 0)
                    val duration = intent.getIntExtra("duration", 0)
                    simulateClick(x.toFloat(), y.toFloat(), duration.toLong())
                }
                SWIPE_ACTION -> {
                    val x = intent.getIntExtra("x", 0)
                    val y = intent.getIntExtra("y", 0)
                    val endX = intent.getIntExtra("endX", 0)
                    val endY = intent.getIntExtra("endY", 0)
                    val duration = intent.getIntExtra("duration", 0)
                    simulateSwipe(x.toFloat(), y.toFloat(), endX.toFloat(), endY.toFloat(), duration.toLong())
                }
            }
        }
    }

    companion object {
        val START_ACTION = "com.chenzhiyong.inputsimulator.start";
        val STOP_ACTION = "com.chenzhiyong.inputsimulator.stop";
        val LOAD_SCRIPT_ACTION = "com.chenzhiyong.inputsimulator.load_script";
        val CLICK_ACTION = "com.chenzhiyong.inputsimulator.click";
        val SWIPE_ACTION = "com.chenzhiyong.inputsimulator.swipe";

    }
}