package edu.tyut.ffmpeglearn.utils

import android.annotation.SuppressLint
import android.content.Context
import android.net.Uri
import android.view.Surface
import androidx.core.content.FileProvider
import java.io.File
import java.io.FileDescriptor
import java.io.InputStream
import java.io.InputStreamReader
import java.io.OutputStream
import java.lang.reflect.Field

internal object Utils {
    @SuppressLint("DiscouragedPrivateApi")
    fun getIntFd(fileDescriptor: FileDescriptor): Int {
        // 用反射访问 FileDescriptor#descriptor 字段
        val fdField: Field = FileDescriptor::class.java.getDeclaredField("descriptor")
        fdField.isAccessible = true
        return fdField.getInt(fileDescriptor)
    }
    internal fun getFileUri(context: Context, fileName: String): Uri {
        val file = File(context.cacheDir, fileName)
        val uri: Uri = FileProvider.getUriForFile(context, "${context.packageName}.provider", file)
        return uri
    }
    internal fun getFileUri(context: Context, file: File): Uri {
        val uri: Uri = FileProvider.getUriForFile(context, "${context.packageName}.provider", file)
        return uri
    }
    internal fun copyToCache(context: Context, input: Uri, output: Uri){
        context.contentResolver.openInputStream(input)?.use { inputStream: InputStream ->
            context.contentResolver.openOutputStream(output)?.use { outputStream: OutputStream ->
                inputStream.copyTo(out = outputStream, bufferSize = 1 shl 10 shl 10)
            }
        }
    }
    init {
        System.loadLibrary("ffmpegLearn")
    }
    /**
     * A native method that is implemented by the 'helloCpp' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun playPcmWithOpenSL(pcmPath: String): Boolean
    external fun nativeRender(surface: Surface): Long
    external fun nativeRenderRelease(ptr: Long)
    external fun nativeTest(): String
    external fun getFfmpegInfo(): String
    external fun getVideoInfo(videoFd: Int): String
    external fun getImageInfo(imageFd: Int): String
    external fun getLameVersion(): String
    external fun convertPcmToMp3(pcmPath: String, mp3Path: String, sampleRate: Int, channels: Int, bitRate: Int): Boolean
    external fun extractYumPcmFromMp4(videoPath: String): Boolean
    external fun getVideoDuration(filePath: String): String
    external fun nativeReadText(fd: Int): String
    external fun nativeReadByte(fd: Int): String
    external fun nativeConvertYuv422PToYuv420P(width: Int, height: Int, srcYuv422p: ByteArray, destYuv420p: ByteArray): Int
    external fun nativeYuv422PToYuv420P(inputPath: String, outputPath: String): Int
    external fun nativeGetSubTitle(inputPath: String, outputPath: String): Int
    external fun nativeGenerateSRTFromVideo(inputPath: String, outputPath: String): Int
    external fun nativeBurnSubtitles(inputPath: String, subTitlePath: String, outputPath: String): Int

}