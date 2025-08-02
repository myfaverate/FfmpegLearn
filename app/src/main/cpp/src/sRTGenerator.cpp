//
// Created by 29051 on 2025/5/18.
//
#include "sRTGenerator.hpp"

constexpr const char *TAG = "sRTGenerator";
/**
 * 生成SRT格式字幕文件
 * @param videoPath 输入视频路径（用于获取时间信息）
 * @param subtitlePath 输出SRT文件路径
 * @param entries 字幕条目数组
 * @param count 字幕条目数量
 * @return 0成功，负数失败
 */
int generateSRT(const char* videoPath, const char* subtitlePath,
                SubtitleEntry* entries, int count) {
    AVFormatContext* fmt_ctx = nullptr;
    int ret = 0;
    FILE* srt_file = nullptr;

    // 1. 打开视频文件获取时间基准
    if ((ret = avformat_open_input(&fmt_ctx, videoPath, nullptr, nullptr)) < 0) {
        logger::error(TAG, "Could not open video file: %s", av_err2str(ret));
        return ret;
    }

    // 获取视频时间基准（用于时间转换）
    AVRational time_base = {1, 1000}; // 默认使用毫秒
    if (fmt_ctx->nb_streams > 0) {
        AVStream* stream = fmt_ctx->streams[0];
        if (stream->time_base.den > 0) {
            time_base = stream->time_base;
        }
    }

    // 2. 创建SRT文件
    srt_file = fopen(subtitlePath, "w");
    if (!srt_file) {
        logger::error(TAG, "Could not create SRT file");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // 3. 写入SRT内容
    for (int i = 0; i < count; i++) {
        SubtitleEntry* entry = &entries[i];

        // 转换时间戳格式 (HH:MM:SS,mmm)
        int start_h = (entry->start_ms / 3600000);
        int start_m = (entry->start_ms / 60000) % 60;
        int start_s = (entry->start_ms / 1000) % 60;
        int start_ms = entry->start_ms % 1000;

        int end_h = (entry->end_ms / 3600000);
        int end_m = (entry->end_ms / 60000) % 60;
        int end_s = (entry->end_ms / 1000) % 60;
        int end_ms = entry->end_ms % 1000;

        // 写入SRT块
        fprintf(srt_file, "%d\n", entry->index);
        fprintf(srt_file, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
                start_h, start_m, start_s, start_ms,
                end_h, end_m, end_s, end_ms);
        fprintf(srt_file, "%s\n\n", entry->text);
    }

    // 4. 清理资源
    fclose(srt_file);
    avformat_close_input(&fmt_ctx);

    logger::info(TAG, "SRT file generated successfully: %s", subtitlePath);
    return 0;
}

/**
 * 从视频中检测语音并自动生成字幕（示例）
 * @param videoPath 输入视频路径
 * @param srtPath 输出SRT路径
 * @return 生成的条目数量（负数表示错误）
 */
int autoGenerateSRTFromVideo(const char* videoPath, const char* srtPath) {
    // 示例：这里应该是语音识别实现
    // 实际应用中需要集成语音识别库如Vosk、PocketSphinx等

    // 模拟生成3条字幕
    SubtitleEntry entries[3] = {
            {1, 1000, 3000, "Hello world"},
            {2, 4000, 6000, "This is a test"},
            {3, 7000, 9000, "Goodbye"}
    };

    if (generateSRT(videoPath, srtPath, entries, 3) < 0) {
        return -1;
    }

    return 3;
}

// 示例使用
// int main() {
//     const char* video = "input.mp4";
//     const char* srt = "output.srt";
//     int count = autoGenerateSRTFromVideo(video, srt);
//     if (count > 0) {
//         printf("Generated %d subtitle entries\n", count);
//     } else {
//         printf("Generation failed\n");
//     }
//
//     return 0;
// }