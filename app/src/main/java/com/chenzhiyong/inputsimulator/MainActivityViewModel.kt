package com.chenzhiyong.inputsimulator

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class MainActivityViewModel : ViewModel() {
    private val consoleText = MutableLiveData<String>("")
    fun getConsoleText() : LiveData<String>{
        return consoleText
    }

    fun appendConsoleText(text : String) {
        consoleText.value += "$text\n"
        consoleText.postValue(consoleText.value)
    }

    fun cleanConsole() {
        consoleText.postValue("")
    }
}