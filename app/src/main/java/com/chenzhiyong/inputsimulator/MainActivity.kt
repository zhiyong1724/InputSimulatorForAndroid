package com.chenzhiyong.inputsimulator
import android.accessibilityservice.AccessibilityServiceInfo
import android.content.BroadcastReceiver
import android.content.Context
import android.content.DialogInterface
import android.content.Intent
import android.content.IntentFilter
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.view.accessibility.AccessibilityManager
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.lifecycle.ViewModelProvider
import kotlin.system.exitProcess

class MainActivity : AppCompatActivity() {
    private lateinit var consoleTextView : TextView
    private lateinit var startButton : TextView
    private lateinit var endButton : TextView
    private lateinit var mainActivityViewModel : MainActivityViewModel
    private var mainActivityBroadcastReceiver : MainActivityBroadcastReceiver? = null
    @RequiresApi(Build.VERSION_CODES.R)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        consoleTextView = findViewById(R.id.consoleTextView)
        startButton = findViewById(R.id.startButton)
        startButton.setOnClickListener{
            var intent = Intent(InputAccessibilityService.LOAD_SCRIPT_ACTION)
            intent.putExtra("script", getPublicDirectory() + "/start.json")
            sendBroadcast(intent)

            intent = Intent(InputAccessibilityService.START_ACTION)
            sendBroadcast(intent)
        }
        endButton = findViewById(R.id.endButton)
        endButton.setOnClickListener{
            finishAffinity()
            exitProcess(0)
        }
        mainActivityViewModel = ViewModelProvider(this).get(MainActivityViewModel::class.java)
        mainActivityViewModel.getConsoleText().observe(this) {
            text : String ->
            consoleTextView.setText(text)
        }
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        checkAndRequestStoragePermission()
        showEnableAccessibilityServiceDialog()

        mainActivityBroadcastReceiver = MainActivityBroadcastReceiver()
        val filter = IntentFilter(AndroidConsoleView.LOG_ACTION)
        registerReceiver(mainActivityBroadcastReceiver, filter, RECEIVER_EXPORTED)
    }

    override fun onDestroy() {
        super.onDestroy()
        val intent = Intent(InputAccessibilityService.STOP_ACTION)
        sendBroadcast(intent)
        if (mainActivityBroadcastReceiver != null) {
            unregisterReceiver(mainActivityBroadcastReceiver)
        }
    }

    private fun appendToConsole(text: String) {
        mainActivityViewModel.appendConsoleText(text)
    }

    private fun cleanConsole() {
        mainActivityViewModel.cleanConsole()
    }

    private fun showEnableAccessibilityServiceDialog() {
        AlertDialog.Builder(this)
            .setTitle(getText(R.string.enable_accessibility_service))
            .setMessage(getText(R.string.enable_accessibility_service_description))
            .setPositiveButton(getText(R.string.to_settings), DialogInterface.OnClickListener { dialog, which ->
                val intent: Intent = Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS)
                startActivity(intent)
            })
            .setNegativeButton(getText(R.string.cancel),
                DialogInterface.OnClickListener { dialog, which ->
                    finishAffinity()
                    exitProcess(0)
                })
            .setCancelable(false)
            .show()
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private fun checkAndRequestStoragePermission() {
        if (!Environment.isExternalStorageManager()) {
            showRequestStoragePermissionDialog()
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private fun showRequestStoragePermissionDialog() {
        AlertDialog.Builder(this)
            .setTitle(getText(R.string.storage_permission_required))
            .setMessage(getText(R.string.storage_permission_required_description))
            .setPositiveButton(getText(R.string.to_settings), DialogInterface.OnClickListener { dialog, which ->
                val intent = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
                startActivity(intent)
            })
            .setNegativeButton(getText(R.string.cancel),
                DialogInterface.OnClickListener { dialog, which ->
                    finishAffinity()
                    exitProcess(0)
                })
            .setCancelable(false)
            .show()
    }

    private fun getPublicDirectory() : String{
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).path
    }

    private inner class MainActivityBroadcastReceiver : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when(intent.action) {
                AndroidConsoleView.LOG_ACTION -> {
                    val text = intent.getStringExtra("text")
                    text?.let {
                        appendToConsole(it)
                    }
                }
            }
        }
    }

    companion object {
        init {
            System.loadLibrary("inputsimulator")
        }
    }
}