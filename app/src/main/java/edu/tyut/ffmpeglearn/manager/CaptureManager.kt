package edu.tyut.ffmpeglearn.manager

import android.Manifest
import android.content.Context
import android.graphics.ImageFormat
import android.hardware.camera2.CameraCaptureSession
import android.hardware.camera2.CameraCharacteristics
import android.hardware.camera2.CameraDevice
import android.hardware.camera2.CameraManager
import android.hardware.camera2.CameraMetadata
import android.hardware.camera2.CaptureFailure
import android.hardware.camera2.CaptureRequest
import android.hardware.camera2.params.OutputConfiguration
import android.hardware.camera2.params.SessionConfiguration
import android.hardware.camera2.params.StreamConfigurationMap
import android.media.Image
import android.media.ImageReader
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.os.Handler
import android.os.HandlerThread
import android.util.Log
import android.util.Range
import android.util.Size
import android.view.Surface
import androidx.annotation.RequiresPermission
import androidx.core.content.FileProvider
import java.io.BufferedOutputStream
import java.io.File
import java.nio.ByteBuffer
import java.util.concurrent.Executor

private const val TAG: String = "CaptureManager"

internal class CaptureManager internal constructor(
    private val context: Context,
){
    private var lastTimestamp = 0L
    private var frameCount = 0
    private val cameraManager: CameraManager by lazy {
        context.getSystemService<CameraManager>(CameraManager::class.java)
    }

    private val cameraThread = HandlerThread("CameraThread").apply { start() }
    private val cameraHandler = Handler(cameraThread.looper)
    private val executor: Executor = Executor { runnable ->
        cameraHandler.post(runnable)
    }

    private var mCaptureSession: CameraCaptureSession? = null
    private var mCameraDevice: CameraDevice? = null
    private var mImageReader: ImageReader? = null

    private val yuv420pUri: Uri = FileProvider.getUriForFile(context, "${context.packageName}.provider", File(
        Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS), "yuv420p.yuv"
    ))


    @RequiresPermission(Manifest.permission.CAMERA)
    // @RequiresApi(Build.VERSION_CODES.VANILLA_ICE_CREAM)
    internal fun open(){
        // val cameraId: String = cameraManager.cameraIdList.firstOrNull() ?: "0"
        val cameraId: String = "0"
        val cameraCharacteristics: CameraCharacteristics = cameraManager.getCameraCharacteristics(cameraId)
        val streamConfigurationMap: StreamConfigurationMap? = cameraCharacteristics[CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP]
        // LEGACY级别表示设备是通过旧版 camera HAL 模拟的 camera2 API
        val isSupportLegacy: Boolean = cameraCharacteristics[CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL] == CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY
        val fpsRanges: Array<Range<Int>>? = cameraCharacteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES)
        fpsRanges?.forEach {
            Log.i(TAG, "Supported FPS range: ${it.lower} - ${it.upper}")
        }
        val hardwareLevel = cameraCharacteristics.get(
            CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL)
        val outputSizes: Array<Size>? = streamConfigurationMap?.getOutputSizes(ImageFormat.YUV_420_888)
        Log.i(TAG, "open -> isSupportLegacy: $isSupportLegacy, hardwareLevel: $hardwareLevel, outputSizes: ${outputSizes?.joinToString()}")
        streamConfigurationMap?.getHighSpeedVideoSizes()?.forEach {
            val ranges = streamConfigurationMap.getHighSpeedVideoFpsRangesFor(it)
            Log.i(TAG, "open -> size: $it, fps: ${ranges.joinToString()}")
        }
        val pixelsSize: Size = outputSizes?.firstOrNull {  it.width == 1280 && it.height == 720 } ?: Size(1280, 720) // 1440 1080
        Log.i(TAG, "open -> pixelsSize: $pixelsSize")
        val imageReader = ImageReader.newInstance(pixelsSize.width, pixelsSize.height, ImageFormat.YUV_420_888, 3)
        this.mImageReader = imageReader

        imageReader.setOnImageAvailableListener({ imageReader: ImageReader? ->
                imageReader?.acquireLatestImage()?.use { image ->
                    Log.i(
                        TAG,
                        "open -> Available image width: ${image.width}, height: ${image.height}"
                    )
                    val yByteBuffer: ByteBuffer? = image.planes?.getOrNull(0)?.buffer
                    val uByteBuffer: ByteBuffer? = image.planes?.getOrNull(1)?.buffer
                    val vByteBuffer: ByteBuffer? = image.planes?.getOrNull(2)?.buffer
                    this@CaptureManager.saveYuv420p(image, yByteBuffer, uByteBuffer, vByteBuffer)
                }
            }, cameraHandler)

        // imageReader.setOnImageAvailableListener({ reader ->
        //     val currentTimestamp = System.currentTimeMillis()
        //     frameCount++
        //     if (lastTimestamp == 0L) {
        //         lastTimestamp = currentTimestamp
        //     } else {
        //         val diff = currentTimestamp - lastTimestamp
        //         if (diff >= 1000) {
        //             val actualFps = frameCount * 1000 / diff
        //             Log.i(TAG, "Actual capture FPS: $actualFps")
        //             frameCount = 0
        //             lastTimestamp = currentTimestamp
        //         }
        //     }
        //     val image = reader.acquireLatestImage()
        //     image?.close()
        // }, cameraHandler)
        cameraManager.openCamera(cameraId, object : CameraDevice.StateCallback() {
            override fun onDisconnected(camera: CameraDevice) {
                Log.i(TAG, "onDisconnected...")
            }

            override fun onError(camera: CameraDevice, error: Int) {
                Log.i(TAG, "onError -> error: $error")
            }

            override fun onOpened(camera: CameraDevice) {
                this@CaptureManager.mCameraDevice = camera

                val captureRequestBuilder: CaptureRequest.Builder = camera.createCaptureRequest(if (isSupportLegacy) CameraDevice.TEMPLATE_RECORD else CameraDevice.TEMPLATE_PREVIEW)
                captureRequestBuilder.addTarget(imageReader.surface)
                fpsRanges?.firstOrNull { it.lower == 30 && it.upper == 30 }?.let { fpsRange: Range<Int> ->
                    Log.i(TAG, "onOpened -> fpsRange: $fpsRange")
                    captureRequestBuilder.set<Range<Int>>(
                        CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE,
                        fpsRange // 这里示例设置为 24fps
                    )
                }
                captureRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON)
                createSession(camera = camera, imageReader = imageReader, captureRequestBuilder = captureRequestBuilder)
            }

            override fun onClosed(camera: CameraDevice) {
                super.onClosed(camera)
                Log.i(TAG, "onClosed...")
                // cameraHandler.post {
                //     cameraHandler.removeCallbacksAndMessages(null)
                //     val quitSafely: Boolean = cameraThread.quitSafely()
                //     Log.i(TAG, "release -> quitSafely: $quitSafely")
                // }
            }
        }, cameraHandler)
    }

    // @RequiresApi(Build.VERSION_CODES.VANILLA_ICE_CREAM)
    private fun createSession(camera: CameraDevice, imageReader: ImageReader, captureRequestBuilder: CaptureRequest.Builder){
        val outputConfiguration = OutputConfiguration(imageReader.surface)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            val configs = mutableListOf<OutputConfiguration>()
            val sessionConfiguration = SessionConfiguration(SessionConfiguration.SESSION_REGULAR, listOf<OutputConfiguration>(outputConfiguration),
                executor, object : CameraCaptureSession.StateCallback(){
                override fun onConfigureFailed(session: CameraCaptureSession) {
                    Log.i(TAG, "onConfigureFailed...")
                }

                override fun onConfigured(session: CameraCaptureSession) {
                    this@CaptureManager.mCaptureSession = session
                    session.setRepeatingRequest(captureRequestBuilder.build(), object : CameraCaptureSession.CaptureCallback(){
                        override fun onCaptureFailed(
                            session: CameraCaptureSession,
                            request: CaptureRequest,
                            failure: CaptureFailure
                        ) {
                            Log.i(TAG, "onCaptureFailed -> failure...")
                        }
                    }, cameraHandler)
                }
            })
            camera.createCaptureSession(sessionConfiguration)
        } else {
            @Suppress("DEPRECATION")
            camera.createCaptureSession(listOf<Surface>(imageReader.surface), object : CameraCaptureSession.StateCallback(){
                override fun onConfigureFailed(session: CameraCaptureSession) {
                    Log.i(TAG, "onConfigureFailed...")
                }
                override fun onConfigured(session: CameraCaptureSession) {
                    this@CaptureManager.mCaptureSession = session
                    session.setRepeatingRequest(captureRequestBuilder.build(), object : CameraCaptureSession.CaptureCallback(){
                        override fun onCaptureFailed(
                            session: CameraCaptureSession,
                            request: CaptureRequest,
                            failure: CaptureFailure
                        ) {
                            Log.i(TAG, "onCaptureFailed -> failure...")
                        }
                    }, cameraHandler)
                }
            }, cameraHandler)
        }
    }
    //  ffplay -f rawvideo -pixel_format yuv420p -video_size 800x600 yuv420p.yuv
    private fun saveYuv420p(
        image: Image,
        yByteBuffer: ByteBuffer?,
        uByteBuffer: ByteBuffer?,
        vByteBuffer: ByteBuffer?
    ){
        val width = image.width
        val height = image.height

        // 打开输出流
        context.contentResolver.openOutputStream(yuv420pUri)?.buffered()?.use { output ->
            // 写 Y 分量
            yByteBuffer?.let { buffer ->
                val rowStride = image.planes?.get(0)?.rowStride ?: width
                val tempRow = ByteArray(width)
                for (row in 0 until height) {
                    buffer.position(row * rowStride)
                    buffer.get(tempRow, 0, width)
                    output.write(tempRow)
                }
            }

            // 写 U 分量
            uByteBuffer?.let { buffer ->
                val rowStride =
                    image.planes?.get(1)?.rowStride ?: (width / 2)
                val pixelStride = image.planes?.get(1)?.pixelStride ?: 1
                val tempRow = ByteArray(width / 2)
                for (row in 0 until height / 2) {
                    buffer.position(row * rowStride)
                    if (pixelStride == 1) {
                        buffer.get(tempRow, 0, width / 2)
                    } else {
                        // 像素之间有间隔时，需要手动取样
                        for (col in 0 until width / 2) {
                            tempRow[col] = buffer.get(col * pixelStride)
                        }
                    }
                    output.write(tempRow)
                }
            }

            // 写 V 分量
            vByteBuffer?.let { buffer ->
                val rowStride = image.planes?.get(2)?.rowStride ?: (width / 2)
                val pixelStride = image.planes?.get(2)?.pixelStride ?: 1
                val tempRow = ByteArray(width / 2)
                for (row in 0 until height / 2) {
                    buffer.position(row * rowStride)
                    if (pixelStride == 1) {
                        buffer.get(tempRow, 0, width / 2)
                    } else {
                        for (col in 0 until width / 2) {
                            tempRow[col] = buffer.get(col * pixelStride)
                        }
                    }
                    output.write(tempRow)
                }
            }

            // 一帧一帧刷写
            output.flush()
        }
    }

    internal fun release(){
        // 1. 停止重复请求

        try {
            mCaptureSession?.stopRepeating()
            mCaptureSession?.abortCaptures()

            mCaptureSession?.close()
            mImageReader?.close()
            mCameraDevice?.close()

            mImageReader?.setOnImageAvailableListener(null, null)

            mCaptureSession = null
            mImageReader = null
            mCameraDevice = null

            cameraHandler.removeCallbacksAndMessages(null)
            val quitSafely: Boolean = cameraThread.quitSafely()
            Log.i(TAG, "release -> quitSafely: $quitSafely")
            cameraThread.join()

        } catch (e: Exception){
            Log.e(TAG, "release -> error: ${e.message}", e)
        }
    }
}