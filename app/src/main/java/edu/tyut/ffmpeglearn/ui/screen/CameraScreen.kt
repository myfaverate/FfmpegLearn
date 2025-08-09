package edu.tyut.ffmpeglearn.ui.screen

import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
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
import androidx.navigation.ActivityNavigatorExtras
import androidx.navigation.NavHostController
import edu.tyut.ffmpeglearn.manager.CaptureManager
import edu.tyut.ffmpeglearn.ui.theme.RoundedCornerShape10
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch

private const val TAG: String = "CameraScreen"

@Composable
internal fun CameraScreen(
    navHostController: NavHostController,
    snackBarHostState: SnackbarHostState,
){
    val context: Context = LocalContext.current
    val coroutineScope: CoroutineScope = rememberCoroutineScope()
    val captureManager: CaptureManager by remember {
        mutableStateOf(value = CaptureManager(context = context))
    }
    val permissions: Array<String> = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        arrayOf(android.Manifest.permission.CAMERA, android.Manifest.permission.READ_MEDIA_VIDEO)
    } else {
        arrayOf(android.Manifest.permission.CAMERA, android.Manifest.permission.READ_EXTERNAL_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE)
    }
    val launcher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) { map ->
        coroutineScope.launch {
            snackBarHostState.showSnackbar("获取权限是否成功: ${map.values.all { it }}")
        }
    }
    DisposableEffect(key1 = Unit) {
        onDispose {
            captureManager.release()
        }
    }
    Column(
        modifier = Modifier
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
                    captureManager.open()
                },
            color = Color.White
        )
    }
}