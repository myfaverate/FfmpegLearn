//
// Created by 29051 on 2025/7/28.
//
#include "FFmpegDecoder.hpp"

constexpr const char *TAG = "FFmpegDecoder";

void extractYumPcmFromMp4(const std::string &videoPath) {

    logger::info(TAG, "videoPath: %s", videoPath.c_str());

    AVFormatContext *avFormatContext = avformat_alloc_context();
    avFormatContext->interrupt_callback.callback = [](void *opaque) -> int {
        const auto isCanceled = static_cast<bool *>(opaque);
        if (isCanceled && *isCanceled) {
            return 1;
        }
        return 0;
    };


    int result = avformat_open_input(&avFormatContext, videoPath.c_str(), av_find_input_format("mp4"), nullptr);
    if (result < 0) {
        avformat_free_context(avFormatContext);
        logger::error(TAG, "打开失败 -> result: %d, error: %s", result, av_err2str(result));
        return;
    }

    result = avformat_find_stream_info(avFormatContext, nullptr);
    if (result < 0) {
        avformat_free_context(avFormatContext);
        avformat_close_input(&avFormatContext);
        logger::error(TAG, "打开stream_info失败...");
        logger::info(TAG, "stream info: %d", result);
        return;
    }

    int videoStreamIndex = -1;
    int audioStreamIndex = -1;

    logger::info(TAG,
                 "共有多少streams: %d, avFormatContext->iformat->name: %s, avFormatContext->iformat->mime_type: %s, biteRate: %d",
                 avFormatContext->nb_streams, avFormatContext->iformat->name,
                 avFormatContext->iformat->mime_type, avFormatContext->bit_rate);
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        AVStream *stream = avFormatContext->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            logger::info(TAG, "视频轨道 %d", i);
            videoStreamIndex = i;
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            logger::info(TAG, "音频轨道 %d", i);
            audioStreamIndex = i;
        } else {
            logger::info(TAG, "其他轨道 %d", i);
        }
    }

    if (videoStreamIndex == -1 || audioStreamIndex == -1) {
        avformat_free_context(avFormatContext);
        avformat_close_input(&avFormatContext);
        logger::error(TAG, "没有视频或语音...");
        return;
    }

    logger::info(TAG, "videoStreamIndex %d, audioStreamIndex: %d", videoStreamIndex,
                 audioStreamIndex);
    const AVStream *videoStream = avFormatContext->streams[videoStreamIndex];
    const AVStream *audioStream = avFormatContext->streams[audioStreamIndex];
    const AVCodec *videoCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    const AVCodec *audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);


    AVCodecContext *videoCodecContext = avcodec_alloc_context3(videoCodec);
    AVCodecContext *audioCodecContext = avcodec_alloc_context3(audioCodec);

    logger::info(TAG, "videoCodecName: %s, audioCodecName: %s", videoCodecContext->codec->name,
                 audioCodecContext->codec->name);

    avcodec_free_context(&videoCodecContext);
    avcodec_free_context(&audioCodecContext);
}