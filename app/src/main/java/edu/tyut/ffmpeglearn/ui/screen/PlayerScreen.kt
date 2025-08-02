package edu.tyut.ffmpeglearn.ui.screen

import android.content.Context
import android.net.Uri
import android.util.Log
import android.view.SurfaceView
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Box
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.viewinterop.AndroidView
import androidx.media3.common.MediaItem
import androidx.media3.common.PlaybackException
import androidx.media3.common.Player
import androidx.media3.exoplayer.ExoPlayer
import edu.tyut.ffmpeglearn.R

private const val TAG: String = "PlayerScreen"

@Composable
internal fun PlayerScreen(
    modifier: Modifier = Modifier,
    videoUri: Uri,
){
    LaunchedEffect(key1 = Unit) {
        Log.i(TAG, "PlayerScreen -> videoUri: $videoUri")
    }
    val context: Context = LocalContext.current
    // context.contentResolver.openFileDescriptor(videoUri, "r")?.use {
    //     it.fd
    // }
    val exoPlayer: ExoPlayer = remember {
        ExoPlayer.Builder(context).build().apply {
            val mediaItem: MediaItem = MediaItem.fromUri(videoUri)
            setMediaItem(mediaItem)
            prepare()
            repeatMode = Player.REPEAT_MODE_ONE
            playWhenReady = true
        }
    }
    var isPlaying: Boolean by remember {
        mutableStateOf(exoPlayer.isPlaying)
    }
    DisposableEffect(key1 = Unit) {
        val eventListener: Player.Listener = object : Player.Listener {
            override fun onPlayerError(error: PlaybackException) {
                Log.e(TAG, "onPlayerError play error: ${error.message}", error)
            }

            override fun onIsPlayingChanged(isPlayingNow: Boolean) {
                isPlaying = isPlayingNow
            }
        }
        exoPlayer.addListener(eventListener)
        onDispose {
            exoPlayer.removeListener(eventListener)
            exoPlayer.release()
        }
    }
    Box(
        modifier = modifier.clickable {
            if (exoPlayer.isPlaying){
                exoPlayer.pause()
            } else {
                exoPlayer.play()
            }
        },
        contentAlignment = Alignment.Center
    ){
        AndroidView(
            factory = { viewContext: Context ->
                SurfaceView(context).apply {
                    exoPlayer.setVideoSurfaceView(this)
                    layoutParams = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT)
                }
            },
        )
        if (!isPlaying) {
            Icon(
                painter = painterResource(R.drawable.play), contentDescription = "播放按钮",
                tint = Color.White
            )
        }
    }
}