package com.chenzhiyong.inputsimulator

class InputSimulator() {
    private var nativeObject : Long = 0
    init {
        nativeObject = newNativeObject()
    }

    fun releaseNativeObject() {
        deleteNativeObject(nativeObject)
    }

    fun start() {
        start(nativeObject)
    }

    fun stop() {
        stop(nativeObject)
    }

    fun loadScript(script : String) : Boolean{
        return loadScript(nativeObject, script)
    }

    private external fun newNativeObject() : Long
    private external fun deleteNativeObject(obj : Long)
    private external fun start(obj : Long)
    private external fun stop(obj : Long)
    private external fun loadScript(obj : Long, script : String) : Boolean

}