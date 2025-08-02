#include "ffmpegLearn.hpp"

constexpr const char* TAG = "ffmpegLearn";

// 一定要加这行 妈的0.0

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello world 世界 from C++";
    logger::info(TAG, "hello World...");
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeTest(
        JNIEnv *env,
        jobject
) {
    std::string hello = "Hello world 世界 from C++";
    logger::info(TAG, "hello World nativeTest...");
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_getFfmpegInfo(
        JNIEnv *env,
        jobject
) {
    std::stringstream version;
    logger::info(TAG, "libAvcodec: %s", AV_STRINGIFY(LIBAVCODEC_VERSION));
    logger::info(TAG, "libAvformat: %s", AV_STRINGIFY(LIBAVFORMAT_VERSION));
    logger::info(TAG, "libAvutil: %s", AV_STRINGIFY(LIBAVUTIL_VERSION));
    logger::info(TAG, "libAvFilter: %s", AV_STRINGIFY(LIBAVFILTER_VERSION));
    logger::info(TAG, "libSwResample: %s", AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    logger::info(TAG, "avcodecConfiguration: %s", avcodec_configuration());
    logger::info(TAG, "avcodecLicense: %s", avcodec_license());
    version << "libAvcodec: " <<  AV_STRINGIFY(LIBAVCODEC_VERSION) << "\n";
    version << "libAvformat: " << AV_STRINGIFY(LIBAVFORMAT_VERSION) << "\n";
    version << "libAvutil: " << AV_STRINGIFY(LIBAVUTIL_VERSION) << "\n";
    version << "libAvFilter: " << AV_STRINGIFY(LIBAVFILTER_VERSION) << "\n";
    version << "avcodecConfiguration: " << avcodec_configuration() << "\n";
    version << "avcodecLicense: " << avcodec_license() << "\n";
    logger::info(TAG, "format: %s", version.str().c_str());
    list_protocols();
    return env->NewStringUTF(version.str().c_str());
}

void list_protocols() {
    void *opaque = nullptr;
    const char *name = nullptr;

    logger::info(TAG, "Input protocols:");
    while ((name = avio_enum_protocols(&opaque, 0))) {
        logger::info(TAG, "  %s", name);  // 这里应当包含 file
    }

    const AVInputFormat *iformat = nullptr;
    while ((iformat = av_demuxer_iterate(&opaque))) {
        logger::info(TAG, "Demuxer: %s", iformat->name);
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeYuv422PToYuv420P(
        JNIEnv *env,
        jobject,
        jstring inputPath,
        jstring outputPath
){
    const char * input = env->GetStringUTFChars(inputPath, nullptr);
    const char * output = env->GetStringUTFChars(outputPath, nullptr);
    int result = transcode(input, output);
    logger::info(TAG, "Utils_nativeYuv422PToYuv420P result: %d", result);
    env->ReleaseStringUTFChars(inputPath, input);
    env->ReleaseStringUTFChars(outputPath, output);
    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeGetSubTitle(
        JNIEnv *env,
        jobject,
        jstring inputPath,
        jstring outputPath
){
    const char * input = env->GetStringUTFChars(inputPath, nullptr);
    const char * output = env->GetStringUTFChars(outputPath, nullptr);
    // int result = getSubtitle(input, output);
    env->ReleaseStringUTFChars(inputPath, input);
    env->ReleaseStringUTFChars(outputPath, output);
    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeGenerateSRTFromVideo(
        JNIEnv *env,
        jobject,
        jstring inputPath,
        jstring outputPath
){
    const char * input = env->GetStringUTFChars(inputPath, nullptr);
    const char * output = env->GetStringUTFChars(outputPath, nullptr);
    // int result = autoGenerateSRTFromVideo(input, output);
    env->ReleaseStringUTFChars(inputPath, input);
    env->ReleaseStringUTFChars(outputPath, output);
    return -1;
}

extern "C" JNIEXPORT jint JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeBurnSubtitles(
        JNIEnv *env,
        jobject,
        jstring inputPath,
        jstring subtitlePath,
        jstring outputPath
){
    const char * input = env->GetStringUTFChars(inputPath, nullptr);
    const char * output = env->GetStringUTFChars(outputPath, nullptr);
    const char * subtitle = env->GetStringUTFChars(subtitlePath, nullptr);
    // int result = burnSubtitles(input, subtitle, output);
    env->ReleaseStringUTFChars(inputPath, input);
    env->ReleaseStringUTFChars(outputPath, output);
    env->ReleaseStringUTFChars(subtitlePath, subtitle);
    return -1;
}



extern "C" JNIEXPORT jint JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeConvertYuv422PToYuv420P(
        JNIEnv *env,
        jobject,
        jint width,
        jint height,
        jbyteArray srcYuv422p,
        jbyteArray destYuv420p
) {
    SwsContext *swsContext = sws_getContext(
            width, height, AV_PIX_FMT_YUV422P,
            width, height, AV_PIX_FMT_YUV420P,
            SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    if (swsContext == nullptr){
        logger::error(TAG, "Failed to create SwsContext");
        return -1;
    }

    // 获取Java数组元素指针
    jbyte* srcData = env->GetByteArrayElements(srcYuv422p, nullptr);
    jbyte* destData = env->GetByteArrayElements(destYuv420p, nullptr);

    if (!srcData || !destData || width <= 0 | height <= 0){
        logger::error(TAG, "srcData or destData is null");
        sws_freeContext(swsContext);
        return -1;
    }

    const int ySize = width * height;
    const int uv422Size = width * height / 2;
    const int uv420Size = (width / 2) * (height / 2);

    // 对YUV422P，平面布局：
    // Y平面宽*高，U平面宽*高，V平面宽*高（根据格式不同，可能有不同）
    // YUV422P每个U,V平面是宽的一半，高度与Y相同
    // YUV420P每个U,V平面是宽和高的一半

    uint8_t* srcPlanes[4] = {
            (uint8_t*) srcData, // y
            (uint8_t*) srcData + ySize, // u
            (uint8_t*) srcData + ySize + uv422Size, // v
    };
    int srcLineSize[4] = {
            width,
            width / 2,
            width/ 2,
    };

    uint8_t* destPlanes[4] = {
            (uint8_t*) destData, // y
            (uint8_t*) destData + ySize, // u
            (uint8_t*) destData + ySize + uv420Size, // v
    };
    int destLineSize[4] = {
            width,
             width / 2,
             width / 2
    };

    logger::info(TAG, "srcPlanes[0]=%p, srcPlanes[1]=%p, srcPlanes[2]=%p",
         srcPlanes[0], srcPlanes[1], srcPlanes[2]);

    // // y plane
    // srcPlanes[0] = (uint8_t*) srcData;
    // srcLineSize[0] = width;
    //
    // // u plane
    // srcPlanes[1] = srcPlanes[0] + width * height;
    // srcLineSize[1] = width / 2;
    //
    // // v
    // srcPlanes[2] = srcPlanes[1] + width / 2 * height;
    // srcLineSize[2] = width / 2;
    //
    // srcPlanes[3] = nullptr;
    // srcLineSize[3] = 0;
    //
    // destPlanes[0] = (uint8_t*) destData;
    // destLineSize[0] = width;
    //
    // destPlanes[1] = destPlanes[0] + width * height;
    // destLineSize[1] = width / 2;
    //
    // destPlanes[2] = destPlanes[1] + width / 2 * height / 2;
    // destLineSize[2] = width / 2;
    //
    // destPlanes[3] = nullptr;
    // destLineSize[3] = 0;
    int result = sws_scale(swsContext, srcPlanes, srcLineSize, 0, height, destPlanes, destLineSize);

    sws_freeContext(swsContext);
    env->ReleaseByteArrayElements(srcYuv422p, srcData, JNI_ABORT);
    env->ReleaseByteArrayElements(destYuv420p, destData, JNI_ABORT);

    if (result < 0){
        logger::error(TAG, "sws_cale failed");
        return -1;
    }

    if (result != height) {
        logger::error(TAG, "Conversion failed: output lines %d (expected %d)", result, height);
        return -1;
    }

    logger::info(TAG, "YUV422P to YUV4220 success...");
    return 0;
}

void log_callback(void* ptr, int level, const char* fmt, va_list vl) {
    if (level <= AV_LOG_INFO) {
        char line[1024];
        vsnprintf(line, sizeof(line), fmt, vl);
        __android_log_print(ANDROID_LOG_INFO, "FFmpeg", "%s", line);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_getVideoDuration(
        JNIEnv* env,
        jobject,
        jstring file_path
) {
    av_log_set_level(AV_LOG_DEBUG); // 在调用前添加

    av_log_set_callback(log_callback);

    list_protocols();

    av_log_set_callback([](void* ptr, int level, const char* fmt, va_list vl) {
        if (level <= AV_LOG_INFO) {
            char log[1024];
            vsnprintf(log, sizeof(log), fmt, vl);
            logger::info(TAG, "log: %s", log);
        }
    });
    const char* path = env->GetStringUTFChars(file_path, nullptr);
    logger::info(TAG, "path: %s", path);
    // 注册所有组件（旧版本需要）
    // av_register_all(); // 新版本 FFmpeg 可能不需要

    AVFormatContext* fmt_ctx = avformat_alloc_context();
    // if (avformat_open_input(&fmt_ctx, path, nullptr, nullptr) != 0) {
    //     env->ReleaseStringUTFChars(file_path, path);
    //     return env->NewStringUTF("Cannot open input file");
    // }

    char err_buf[AV_ERROR_MAX_STRING_SIZE] = {0};
    int ret = avformat_open_input(&fmt_ctx, path, nullptr, nullptr);
    if (ret < 0) {
        av_strerror(ret, err_buf, sizeof(err_buf));
        logger::info(TAG, "avformat_open_input failed: %s", err_buf);  // 关键！
        env->ReleaseStringUTFChars(file_path, path);
        return env->NewStringUTF("open input failed");
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        avformat_close_input(&fmt_ctx);
        env->ReleaseStringUTFChars(file_path, path);
        return env->NewStringUTF("Cannot find stream info");
    }

    int64_t duration = fmt_ctx->duration; // 单位是微秒
    char info[256];
    snprintf(info, sizeof(info), "Duration: %.2f seconds", (double)duration / (double)AV_TIME_BASE);

    avformat_close_input(&fmt_ctx);
    env->ReleaseStringUTFChars(file_path, path);
    return env->NewStringUTF(info);
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeReadByte(
        JNIEnv *env,
        jobject,
        jint fd
) {
    FILE* file = fdopen(fd, "rb");
    if (file == nullptr){
        return env->NewStringUTF("file is null");
    }
    constexpr size_t bufferSize = 1024UL;
    char buffer[bufferSize];
    size_t byteRead = 0;
    while((byteRead = fread(buffer, 1, bufferSize, file)) > 0){
        // content.append(buffer, byteRead);
        for (size_t i = 0; i < byteRead; ++i) {
            logger::info(TAG, "Byte[%zu] = 0x%02X", i, (unsigned char)buffer[i]);
        }
        break;
    }
    fclose(file);
    return env->NewStringUTF("read success");
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeReadText(
        JNIEnv *env,
        jobject,
        jint fd
) {
    FILE* file = fdopen(fd, "rb");
    if (file == nullptr){
        return env->NewStringUTF("file is null");
    }
    std::string content;
    constexpr size_t bufferSize = 1024UL;
    char buffer[bufferSize];
    size_t byteRead = 0;
    while((byteRead = fread(buffer, 1, bufferSize, file)) > 0){
        content.append(buffer, byteRead);
    }
    fclose(file);
    return env->NewStringUTF(content.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_getVideoInfo(
        JNIEnv *env,
        jobject,
        jint videoFd
) {
    // av_log_set_level(AV_LOG_DEBUG);
    // if (videoFd < 0) {
    //     return env->NewStringUTF("invalid file descriptor");
    // }
    // constexpr int bufferSize = 4096;
    // auto* avIoBuffer = (unsigned char *) av_malloc(bufferSize);
    // if (avIoBuffer == nullptr){
    //     return env ->NewStringUTF("avIo buffer alloc failed");
    // }
    //
    // int duplicateFd = dup(videoFd);
    //
    // if (duplicateFd < 0) {
    //     av_free(avIoBuffer);
    //     return env ->NewStringUTF("dump fd failed");
    // }
    //
    // AVIOContext *avIo = avio_alloc_context(
    //         avIoBuffer,
    //         bufferSize,
    //         0,
    //         (void *) (intptr_t) duplicateFd,
    //         [](void *opaque, uint8_t *buf, int buf_size) -> int {
    //             int fd = (int)(intptr_t) opaque;
    //             ssize_t bytesRead = read(fd, buf, buf_size);
    //             logger::info(TAG, "bytesRead: %ld", bytesRead);
    //             return (int)bytesRead;
    //         },
    //         nullptr,
    //         [](void *opaque, int64_t offset, int whence) -> int64_t {
    //             int fd = (int) (intptr_t) opaque;
    //             if (whence == AVSEEK_SIZE){
    //                 struct stat st{};
    //                 if (fstat(fd, &st) == 0){
    //                     return st.st_size;
    //                 } else {
    //                     return -1;
    //                 }
    //             }
    //             return lseek(fd, offset, whence);
    //         }
    // );
    //
    // if (avIo == nullptr){
    //     close(duplicateFd);
    //     av_free(avIoBuffer);
    //     return  env ->NewStringUTF("avIo_alloc_context failed");
    // }
    //
    // AVFormatContext* avFormatContext = avformat_alloc_context();
    //
    // if (avFormatContext == nullptr){
    //     avio_context_free(&avIo);
    //     close(duplicateFd);
    //     av_free(avIoBuffer);
    //     return env->NewStringUTF("avFormat_alloc_context_failed");
    // }
    //
    // avFormatContext -> pb = avIo;
    // avFormatContext->flags = AVFMT_FLAG_CUSTOM_IO;
    //
    // if (avformat_open_input(&avFormatContext, nullptr, nullptr, nullptr) != 0) {
    //     avformat_free_context(avFormatContext);
    //     // av_free(avIoBuffer);
    //     avio_context_free(&avIo);
    //     close(duplicateFd);
    //     return env ->NewStringUTF("open input failed");
    // }
    //
    // int result = avformat_find_stream_info(avFormatContext, nullptr);
    // if (result < 0) {
    //     avformat_close_input(&avFormatContext);
    //     // av_free(avIoBuffer);
    //     avio_context_free(&avIo);
    //     close(duplicateFd);
    //
    //     char errBuf[128];
    //     av_strerror(result, errBuf, sizeof(errBuf));
    //     logger::info(TAG, "avformat_find_stream_info failed: %s", errBuf);
    //     // 资源释放...
    //     return env ->NewStringUTF("find_stream_info failed");
    // }
    //
    // int64_t durationUs = avFormatContext -> duration;
    // double durationSec = durationUs > 0 ? static_cast<double>(durationUs) / (double )AV_TIME_BASE : -1;
    //
    // int width = 0, height = 0;
    // double fps = 0.0;
    // const char *codeName = "unknown";
    //
    // for (unsigned int i = 0; i < avFormatContext -> nb_streams; ++ i) {
    //     AVStream *avStream = avFormatContext -> streams[i];
    //     if (avStream -> codecpar -> codec_type == AVMEDIA_TYPE_VIDEO){
    //         width = avStream -> codecpar -> width;
    //         height = avStream -> codecpar -> height;
    //         if (avStream -> avg_frame_rate.den != 0) {
    //             fps = av_q2d(avStream -> avg_frame_rate);
    //         }
    //         const AVCodec *avCodec = avcodec_find_decoder(avStream -> codecpar -> codec_id);
    //         if (avCodec != nullptr) {
    //             codeName = avCodec->name;
    //         }
    //         break;
    //     }
    // }
    //
    // long bitRate = avFormatContext->bit_rate;
    //
    // avformat_close_input(&avFormatContext);
    // // av_free(avIoBuffer);
    // avio_context_free(&avIo);
    // close(duplicateFd);
    //
    // char info[256];
    //
    // snprintf(info, sizeof(info),
    //          "Duration: %.2f sec\n"
    //          "Resolution: %dx%d\n"
    //          "FPS: %.2f\n"
    //          "Bitrate: %ld bps\n"
    //          "Codec: %s\n",
    //          durationSec,
    //          width,
    //          height,
    //          fps,
    //          bitRate,
    //          codeName
    //          );

    return env->NewStringUTF("hello");
}

extern "C" JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_getImageInfo(
        JNIEnv *env,
        jobject,
        jint imageFd
) {
    logger::info(TAG, "unsigned char: %lu", sizeof(unsigned char));
    logger::info(TAG, "android api: %d", __ANDROID_API__);
#if __ANDROID_API__ >= 30
    if (imageFd < 0){
        return env ->NewStringUTF("error");
    }
    AImageDecoder* decoder;
    int result = AImageDecoder_createFromFd(imageFd, &decoder);
    if (result != ANDROID_IMAGE_DECODER_SUCCESS){
        return env ->NewStringUTF("decode failure");
    }
    auto decoderCleanUp = [&decoder](){
        AImageDecoder_delete(decoder);
    };
    const AImageDecoderHeaderInfo* headerInfo = AImageDecoder_getHeaderInfo(decoder);
    int bitmapFormat = AImageDecoderHeaderInfo_getAndroidBitmapFormat(headerInfo);
    logger::info("bitmapFormat: %d", bitmapFormat);
    if (bitmapFormat != ANDROID_BITMAP_FORMAT_RGBA_8888){
        decoderCleanUp();
        return  env ->NewStringUTF("仅支持RGBA_8888");
    }
    constexpr int kChannels = 4;
    int width = AImageDecoderHeaderInfo_getWidth(headerInfo);
    int height = AImageDecoderHeaderInfo_getHeight(headerInfo);
    size_t stride = AImageDecoder_getMinimumStride(decoder);
    size_t size = width * height * kChannels;
    auto pixels = std::make_unique<uint8_t[]>(size);
    int decodeResult = AImageDecoder_decodeImage(decoder, pixels.get(), stride, size);
    if (decodeResult != ANDROID_IMAGE_DECODER_SUCCESS){
        decoderCleanUp();
        return env ->NewStringUTF("DECODER_ERROR");
    }
    decoderCleanUp();
    return env->NewStringUTF(std::format("width: {}, height: {}, channels: {}, stride: {}", width, height, kChannels, stride).c_str());
#else

    return env->NewStringUTF("API < 30 not supported");
#endif
}


// get_lame_version Hello World
extern "C"
JNIEXPORT jstring JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_getLameVersion(JNIEnv *env, jobject thiz) {
    logger::info(TAG, "lameVersin: %s", get_lame_version());
    return env->NewStringUTF(std::format("lameVersion: {}", get_lame_version()).c_str());
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_convertPcmToMp3(JNIEnv *env, jobject thiz, jstring pcm_path,
                                                      jstring mp3_path, jint sample_rate,
                                                      jint channels, jint bit_rate) {
    try {
        const char * pcmPath = env->GetStringUTFChars(pcm_path, nullptr);
        const char * mp3Path = env->GetStringUTFChars(mp3_path, nullptr);
        const auto mp3Encoder = std::make_unique<Mp3Encoder>(pcmPath, mp3Path, sample_rate, channels, bit_rate);
        mp3Encoder->encode();
        env->ReleaseStringUTFChars(pcm_path, pcmPath);
        env->ReleaseStringUTFChars(mp3_path, mp3Path);
        logger::info(TAG, "success: pcmPath: %s, mp3Path: %s", pcmPath, mp3Path);
        return true;
    }catch(const std::exception &e){
        logger::error(TAG, "error: %s", e.what());
        return false;
    }
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_extractYumPcmFromMp4(JNIEnv *env, jobject thiz,
                                                           jstring video_path) {
    jboolean isCopy = JNI_TRUE;
    const char * videPath = env->GetStringUTFChars(video_path, &isCopy);
    logger::info(TAG, "是否是直接拷贝: %d", isCopy == JNI_TRUE);
    extractYumPcmFromMp4(videPath);
    env->ReleaseStringUTFChars(video_path, videPath);
    return true;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_playPcmWithOpenSL(JNIEnv *env, jobject thiz,
                                                        jstring pcm_path) {
    const char *pcmPath = env->GetStringUTFChars(pcm_path, nullptr);
    playPcm(pcmPath);
    env->ReleaseStringUTFChars(pcm_path, pcmPath);
    return true;
}
extern "C"
JNIEXPORT jlong JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeRender(JNIEnv *env, jobject thiz, jobject surface) {
    if (surface == nullptr){
        logger::info(TAG, "surface is null");
    }
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr){
        logger::info(TAG, "window is null");
        return 0;
    }
    auto openGLLearn = new OpenGLLearn(window);
    openGLLearn->render();
    return reinterpret_cast<jlong>(openGLLearn);;
}
extern "C"
JNIEXPORT void JNICALL
Java_edu_tyut_ffmpeglearn_utils_Utils_nativeRenderRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto* render = reinterpret_cast<OpenGLLearn*>(ptr);
    delete render;
}