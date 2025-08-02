package edu.tyut.ffmpeglearn.manager

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import android.net.Uri
import android.util.Log
import androidx.core.app.ActivityCompat
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.OutputStream

private const val TAG: String = "AudioRecordManager"

internal class AudioRecordManager internal constructor(
    private val context: Context
) {
    // 立体声音
    // ffplay -f s16le -ar 44100 -ch_layout stereo -i pcm1.pcm
    // ffplay -f rawvideo -vf format=yuv420p -video_size 662x1280 output.yuv
    // git branch --set-upstream-to=origin/<远程分支名> <本地分支名>
    private val channelMask: Int = AudioFormat.CHANNEL_IN_STEREO
    private val sampleRate = 44100
    private val bufferSize: Int =
        AudioRecord.getMinBufferSize(sampleRate, channelMask, AudioFormat.ENCODING_PCM_16BIT)

    private val audioRecord: AudioRecord by lazy {
        initAudioRecord()
    }

    private fun initAudioRecord(): AudioRecord {
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.RECORD_AUDIO
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            throw RuntimeException("Not RECORD_AUDIO permission...")
        }
        return AudioRecord.Builder()
            .setAudioSource(MediaRecorder.AudioSource.MIC)
            .setAudioFormat(
                AudioFormat.Builder().setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setSampleRate(sampleRate).setChannelMask(channelMask).build()
            )
            .setBufferSizeInBytes(bufferSize)
            .build()
    }
    internal suspend fun startRecord(uri: Uri): Unit = withContext(Dispatchers.IO){
        audioRecord.startRecording()
        context.contentResolver.openOutputStream(uri)?.use { outputStream: OutputStream ->
                var totalLength = 0L
                val bytes = ByteArray(bufferSize)
                var length: Int
                while (audioRecord.read(bytes, 0, bytes.size).also { length = it } > 0) {
                    Log.i(TAG, "startRecord -> data: ${bytes.joinToString()}")
                    outputStream.write(bytes, 0, length)
                    totalLength += length
                }
                outputStream.flush()
                Log.i(TAG, "startRecord -> 录制完成, 文件大小为: $totalLength bytes")
            }
    }
    // @RequiresPermission(value = Manifest.permission.RECORD_AUDIO)
    internal fun stopRecord(){
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.RECORD_AUDIO
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            throw RuntimeException("Not RECORD_AUDIO permission...")
        }
        if (audioRecord.recordingState == AudioRecord.RECORDSTATE_RECORDING) {
            audioRecord.stop()
        }
    }
    internal fun release(){
        if (ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.RECORD_AUDIO
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            throw RuntimeException("Not RECORD_AUDIO permission...")
        }
        audioRecord.release()
    }
}