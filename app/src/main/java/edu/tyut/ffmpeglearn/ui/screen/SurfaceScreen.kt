package edu.tyut.ffmpeglearn.ui.screen

import android.util.Log
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.ViewGroup
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.SnackbarHostState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableLongStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import androidx.navigation.NavHostController
import edu.tyut.ffmpeglearn.utils.Utils

private const val TAG: String = "SurfaceScreen"

@Composable
internal fun SurfaceScreen(
    navHostController: NavHostController,
    snackBarHostState: SnackbarHostState,
) {
    var ptr: Long by remember {
        mutableLongStateOf(value = 0)
    }
    Box(
        modifier = Modifier.fillMaxSize()
    ) {
        AndroidView(
            factory = { viewContext ->
                SurfaceView(viewContext).apply {
                    layoutParams = ViewGroup.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT
                    )
                    holder.addCallback(object : SurfaceHolder.Callback2{
                        override fun surfaceChanged(
                            holder: SurfaceHolder,
                            format: Int,
                            width: Int,
                            height: Int
                        ) {
                            Log.i(TAG, "surfaceChanged -> format: $format, width: $width, height: $height")
                        }

                        override fun surfaceCreated(holder: SurfaceHolder) {
                            Log.i(TAG, "surfaceCreated...")
                            ptr = Utils.nativeRender(holder.surface)
                        }

                        override fun surfaceDestroyed(holder: SurfaceHolder) {
                            Log.i(TAG, "surfaceDestroyed...")
                            Utils.nativeRenderRelease(ptr = ptr)
                        }

                        override fun surfaceRedrawNeeded(holder: SurfaceHolder) {
                            Log.i(TAG, "surfaceRedrawNeeded...")
                        }
                    })
                }
            },
            update = { surfaceView ->

            }
        )
    }
}