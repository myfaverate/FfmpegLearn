package edu.tyut.ffmpeglearn.ui.screen

import android.content.Context
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.util.Log
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.SnackbarHostState
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.content.ContextCompat
import androidx.core.content.FileProvider
import androidx.lifecycle.ViewModelStoreOwner
import androidx.lifecycle.viewmodel.compose.LocalViewModelStoreOwner
import androidx.navigation.NavHostController
import edu.tyut.ffmpeglearn.manager.AudioRecordManager
import edu.tyut.ffmpeglearn.ui.theme.RoundedCornerShape10
import edu.tyut.ffmpeglearn.utils.Utils
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import java.io.File

private const val TAG: String = "Mp3Screen"

@Composable
internal fun Mp3Screen(
    navHostController: NavHostController,
    snackBarHostState: SnackbarHostState,
){
    val coroutineScope: CoroutineScope = rememberCoroutineScope()
    val permissions: Array<String> = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        arrayOf(android.Manifest.permission.RECORD_AUDIO, android.Manifest.permission.READ_MEDIA_AUDIO)
    } else {
        arrayOf(android.Manifest.permission.RECORD_AUDIO, android.Manifest.permission.WRITE_EXTERNAL_STORAGE, android.Manifest.permission.READ_EXTERNAL_STORAGE)
    }
    val context: Context = LocalContext.current
    val launcher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) { map ->
        coroutineScope.launch {
            snackBarHostState.showSnackbar("权限获取是否成功: ${map.values.all { it }}")
        }
    }
    val audioRecordManager: AudioRecordManager by remember {
        mutableStateOf(value = AudioRecordManager(context))
    }
    DisposableEffect(key1 = Unit) {
        onDispose {
            audioRecordManager.release()
        }
    }
    Column(
        modifier = Modifier.fillMaxSize()
    ) {
        Text(
            text = "显示lame版本",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    coroutineScope.launch {
                        val lameVersion: String = Utils.getLameVersion()
                        snackBarHostState.showSnackbar("lameVersion: $lameVersion")
                        Log.i(TAG, "Mp3Screen -> lameVersion: $lameVersion")
                    }
                },
            color = Color.White
        )
        Text(
            text = "录制pcm 44100 音频 2 声道",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    // 录音读写权限
                    if (permissions.any { ContextCompat.checkSelfPermission(context, it) != PackageManager.PERMISSION_GRANTED}){
                        launcher.launch(permissions)
                        return@clickable
                    }
                    val uri: Uri = FileProvider.getUriForFile(context, "${context.packageName}.provider",
                        File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "pcm1.pcm")
                    )
                    coroutineScope.launch {
                        audioRecordManager.startRecord(uri)
                    }
                },
            color = Color.White
        )
        Text(
            text = "停止录音",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    audioRecordManager.stopRecord()
                },
            color = Color.White
        )
        Text(
            text = "转为mp3",
            Modifier
                .padding(top = 10.dp)
                .background(color = Color.Black, shape = RoundedCornerShape10)
                .padding(all = 5.dp)
                .clickable {
                    val pcmPath = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "pcm1.pcm")
                    val mp3Path = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "pcm1.mp3")
                    val isSuccess: Boolean = Utils.convertPcmToMp3(pcmPath.absolutePath, mp3Path.absolutePath, 44100, 2, 128000)
                    coroutineScope.launch {
                        snackBarHostState.showSnackbar("是否成功: $isSuccess")
                    }
                },
            color = Color.White
        )
    }
}