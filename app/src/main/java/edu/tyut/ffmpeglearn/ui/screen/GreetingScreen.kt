package edu.tyut.ffmpeglearn.ui.screen

import android.annotation.SuppressLint
import android.content.ContentProvider
import android.content.ContentResolver
import android.content.Context
import android.content.pm.PackageManager
import android.media.MediaMetadataRetriever
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.os.ParcelFileDescriptor
import android.system.Os
import android.telephony.mbms.FileInfo
import android.util.Log
import android.widget.Toast
import androidx.activity.compose.ManagedActivityResultLauncher
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.SnackbarHostState
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.core.content.ContextCompat
import androidx.core.net.toUri
import androidx.navigation.NavHostController
import androidx.navigation.compose.rememberNavController
import edu.tyut.ffmpeglearn.R
import edu.tyut.ffmpeglearn.route.Routes
import edu.tyut.ffmpeglearn.ui.theme.FfmpegLearnTheme
import edu.tyut.ffmpeglearn.utils.Utils
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream
import java.lang.reflect.Field

private const val TAG: String = "Greeting"

@OptIn(ExperimentalStdlibApi::class)
@Composable
internal fun Greeting(
    name: String, modifier: Modifier = Modifier,
    snackBarHostState: SnackbarHostState,
    navHostController: NavHostController,
) {
    val context: Context = LocalContext.current
    val coroutineScope: CoroutineScope = rememberCoroutineScope()
    val permission:  ManagedActivityResultLauncher<Array<String>, Map<String, @JvmSuppressWildcards Boolean>> = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestMultiplePermissions()
    ) { map: Map<String, @JvmSuppressWildcards Boolean>  ->
        coroutineScope.launch {
            snackBarHostState.showSnackbar("权限是否获取: $map")
        }
    }
    permission
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(state = rememberScrollState())
    ){
        Text(
            text = "Hello $name",
            modifier = Modifier.clickable {
                val file: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM), "Camera/VID_20250412_110942.mp4")
                Log.i(TAG, "Greeting -> 文件是否存在: ${file.exists()}, 文件大小: ${file.length() / 1024 / 1024}M")
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    if(ContextCompat.checkSelfPermission(context, android.Manifest.permission.READ_MEDIA_VIDEO) != PackageManager.PERMISSION_GRANTED){
                        permission.launch(arrayOf(android.Manifest.permission.READ_MEDIA_VIDEO))
                        return@clickable
                    }
                } else {
                    if(ContextCompat.checkSelfPermission(context, android.Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
                        permission.launch(arrayOf(android.Manifest.permission.READ_EXTERNAL_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE))
                        return@clickable
                    }
                }
                // Log.i(TAG, "Greeting -> 具有读权限")
                // FileInputStream(file).use { input ->
                //     val bytes = ByteArray(size = 1024)
                //     input.read(bytes)
                //     Log.i(TAG, "Greeting -> bytes: ${bytes.toHexString()}")
                // }

                // val file = File(context.cacheDir, "hello.txt")
                // file.writeText(text = "Hello World", charset = Charsets.UTF_8)
                // val readText: String = file.readText(charset = Charsets.UTF_8)
                // Log.i(TAG, "Greeting -> readText: $readText")
                // val pfd: ParcelFileDescriptor = ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_ONLY)
                // val dupPfd: FileDescriptor = Os.dup(pfd.fileDescriptor)
                // val nativeText: String = Utils.nativeReadText(fd = Utils.getIntFd(dupPfd))
                // pfd.close()
                // Log.i(TAG, "Greeting -> nativeText: $nativeText")

                // val file = File(context.cacheDir, "hello.txt")
                // file.writeText(text = "Hello World", charset = Charsets.UTF_8)
                // val readText: String = file.readText(charset = Charsets.UTF_8)
                // Log.i(TAG, "Greeting -> readText: $readText")
                // val uri: Uri = Utils.getFileUri(context = context, fileName = file.name)
                // context.contentResolver.openFileDescriptor(uri, "r")?.use {
                //     val dupPfd: FileDescriptor = Os.dup(it.fileDescriptor)
                //     val nativeText: String = Utils.nativeReadText(fd = Utils.getIntFd(fileDescriptor = dupPfd))
                //     Log.i(TAG, "Greeting -> nativeText: $nativeText")
                // }

                // 读取二进制文件
                // val pfd: ParcelFileDescriptor = ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_ONLY)
                // val dupPfd: FileDescriptor = Os.dup(pfd.fileDescriptor)
                // val nativeText: String = Utils.nativeReadByte(fd = Utils.getIntFd(dupPfd))
                // Log.i(TAG, "Greeting -> nativeTextByte: $nativeText")
                // pfd.close()

                // uri 读取内容不需要权限, fd, file必须要权限
                // val pfd: ParcelFileDescriptor = ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_ONLY)
                // // val dupPfd: FileDescriptor = Os.dup(pfd.fileDescriptor)
                // // val videoDuration: String = Utils.getVideoDuration(file.absolutePath)
                // val videoDuration: String = Utils.getVideoInfo(videoFd = pfd.fd)
                // Log.i(TAG, "Greeting -> videoDuration: $videoDuration")
                // pfd.close()

                // val videoUri: Uri = Uri.Builder().scheme(ContentResolver.SCHEME_ANDROID_RESOURCE)
                //     .authority(context.packageName)
                //     .path(R.raw.question1.toString()).build()
                // val output: Uri = Utils.getFileUri(context = context, fileName = "video1.mp4")
                // Utils.copyToCache(context = context, input = videoUri, output = output)
                // context.contentResolver.openFileDescriptor(output, "r")?.use { pfd ->
                //     val videoInfo: String = Utils.getVideoInfo(pfd.fd)
                //     Log.i(TAG, "Greeting -> videoInfo: $videoInfo")
                // }

                val videoDuration: String = Utils.getVideoDuration(filePath = file.absolutePath)
                Log.i(TAG, "Greeting -> videoDuration: $videoDuration")

            /*
                2025-05-18 00:54:26.445 18599-18599 PlayerScreen            edu.tyut.ffmpeglearn                 E  onPlayerError play error: MediaCodecVideoRenderer error, index=0, format=Format(1, null, video/mp4, video/avc, avc1.7A001F, 1009816, und, [1280, 720, 23.976027, ColorInfo(Unset color space, Unset color range, Unset color transfer, false, 8bit Luma, 8bit Chroma)], [-1, -1]), format_supported=NO_EXCEEDS_CAPABILITIES
                    androidx.media3.exoplayer.ExoPlaybackException: MediaCodecVideoRenderer error, index=0, format=Format(1, null, video/mp4, video/avc, avc1.7A001F, 1009816, und, [1280, 720, 23.976027, ColorInfo(Unset color space, Unset color range, Unset color transfer, false, 8bit Luma, 8bit Chroma)], [-1, -1]), format_supported=NO_EXCEEDS_CAPABILITIES
                        at androidx.media3.exoplayer.ExoPlayerImplInternal.handleMessage(ExoPlayerImplInternal.java:745)
                        at android.os.Handler.dispatchMessage(Handler.java:102)
                 这种错误是因为 android 移动端不支持 YUV422P
                 解法:
                 1. YUV422P  === 一口气 ===> YUV420P
                 2. YUV422P  === 一帧一帧 ===> YUV420P
             */

                // val uri = "content://edu.tyut.ffmpeglearn.provider/cache/video.mp4"
                // navHostController.navigate(route = Routes.Player(videoUri = uri))
                // val retriever: MediaMetadataRetriever = MediaMetadataRetriever().apply {
                //     setDataSource(context, uri.toUri())
                // }
                // val width: Int =
                //     retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH)
                //         ?.toIntOrNull() ?: 0
                // val height: Int =
                //     retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT)
                //         ?.toIntOrNull() ?: 0
                // Log.i(TAG, "Greeting -> width: $width, height: $height")
                // retriever.close()

                /**
                 * 处理 YUV422P
                 */
                val videoUri: Uri = Uri.Builder().scheme(ContentResolver.SCHEME_ANDROID_RESOURCE)
                    .authority(context.packageName)
                    .path(R.raw.question1.toString()).build()
                val output: Uri = Utils.getFileUri(context = context, "question1.mp4")
                val inputPath: String = File(context.cacheDir,  "question1.mp4").absolutePath
                val outputPath: String = File(context.cacheDir,  "outQuestion1.mp4").absolutePath
                Log.i(TAG, "Greeting -> inputPath: $inputPath, outputPath: $outputPath")
                Utils.copyToCache(context = context, input = videoUri, output = output)
                coroutineScope.launch(Dispatchers.IO){

                    val result: Int = Utils.nativeYuv422PToYuv420P(inputPath = inputPath, outputPath = outputPath)
                    Log.i(TAG, "Greeting -> result: ${Utils.getFfmpegInfo()}")
                    snackBarHostState.showSnackbar("success")
                }

                // /data/user/0/edu.tyut.ffmpeglearn/cache/outQuestion1.mp4
                // val outVideoUri: Uri = Utils.getFileUri(context = context, fileName = "outQuestion1.mp4")
                // context.contentResolver.openInputStream(outVideoUri)?.use {
                //     val outFile: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "outVideo.mp4").apply {
                //         val isSuccess: Boolean? = parentFile?.mkdirs()
                //         Log.i(TAG, "Greeting -> isSuccess: $isSuccess")
                //     }
                //     val outUri = Utils.getFileUri(context = context, file = outFile)
                //     context.contentResolver.openOutputStream(outUri)?.use { outputStream ->
                //         it.copyTo(outputStream)
                //     }
                // }
                // // Toast.makeText(context, "success", Toast.LENGTH_SHORT).show()
                // navHostController.navigate(route = Routes.Player(videoUri = Utils.getFileUri(context = context, fileName = "outQuestion1.mp4").toString()))

                // 拿取字幕
                // val outFile: File = File(context.cacheDir, "video.mp4")
                // val videoUri: Uri = Uri.Builder().scheme(ContentResolver.SCHEME_ANDROID_RESOURCE)
                //     .authority(context.packageName)
                //     .path(R.raw.video1.toString()).build()
                // context.contentResolver.openInputStream(videoUri)?.use { inputStream ->
                //     context.contentResolver.openOutputStream(Utils.getFileUri(context = context, file = outFile))?.use { outputStream ->
                //         inputStream.copyTo(out = outputStream)
                //     }
                // }
                // val result = Utils.nativeGetSubTitle(inputPath = outFile.absolutePath, outputPath = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "output.srt").absolutePath)
                // Log.i(TAG, "Greeting -> result :$result")

                // 生成 str 字幕
                // val inFile: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "outVideo.mp4")
                // // val outFile: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "video1.srt")
                // val outFile: File = File(context.cacheDir, "video1.srt")
                // val result = Utils.nativeGenerateSRTFromVideo(inputPath = inFile.absolutePath, outputPath = outFile.absolutePath)
                // Log.i(TAG, "Greeting -> result: $result")


                // # 输出字幕到 视频
                // val input: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "outVideo.mp4")
                // val output: File = File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "outVideoSrt.mp4")
                // val subtitle: File = File(context.cacheDir, "video1.srt")
                // val result = Utils.nativeBurnSubtitles(
                //     inputPath = input.absolutePath,
                //     subTitlePath = subtitle.absolutePath,
                //     outputPath = output.absolutePath
                // )
                // Log.i(TAG, "Greeting -> result: $result")
            }
        )
    }
}

@Preview(showBackground = true)
@Composable
private fun GreetingPreview() {
    FfmpegLearnTheme {
        val navHostController: NavHostController = rememberNavController()
        val snackBarHostState: SnackbarHostState = remember { SnackbarHostState() }
        Greeting(name = "Android", navHostController = navHostController, snackBarHostState = snackBarHostState)
    }
}