package edu.tyut.ffmpeglearn.ui.screen

import android.content.Context
import android.content.pm.PackageManager
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
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.content.ContextCompat
import androidx.core.os.EnvironmentCompat
import androidx.navigation.ActivityNavigatorExtras
import androidx.navigation.NavHostController
import edu.tyut.ffmpeglearn.ui.theme.RoundedCornerShape10
import edu.tyut.ffmpeglearn.utils.Utils
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import java.io.File

private const val TAG: String = "Mp4Screen"

@Composable
internal fun Mp4Screen(
    navHostController: NavHostController,
    snackBarHostState: SnackbarHostState,
) {
    val context: Context = LocalContext.current
    val coroutineScope: CoroutineScope = rememberCoroutineScope()
    val permissions: Array<String> = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        arrayOf(android.Manifest.permission.READ_MEDIA_VIDEO)
    } else {
        arrayOf(
            android.Manifest.permission.READ_EXTERNAL_STORAGE,
            android.Manifest.permission.WRITE_EXTERNAL_STORAGE
        )
    }
    val launcher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) { map ->
        coroutineScope.launch {
            snackBarHostState.showSnackbar("是否获取权限: ${map.values.all { it }}")
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
                    if (permissions.any {
                            ContextCompat.checkSelfPermission(
                                context,
                                it
                            ) != PackageManager.PERMISSION_GRANTED
                        }) {
                        launcher.launch(permissions)
                        return@clickable
                    }
                    val videoPath = File(
                        Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                        "video1.mp4"
                    ).absolutePath
                    // Utils.extractYumPcmFromMp4(videoPath = videoPath)
                    Utils.getFfmpegInfo()
                    Log.i(TAG, "Mp4Screen -> duration: ${Utils.getVideoDuration("/sdcard/DCIM/Camera/VID_20250729_235725.mp4")}")
                },
            color = Color.White
        )
    }
}