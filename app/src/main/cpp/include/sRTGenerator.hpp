//
// Created by 29051 on 2025/8/2.
//

#ifndef FFMPEGLEARN_SRTGENERATOR_HPP
#define FFMPEGLEARN_SRTGENERATOR_HPP
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "logging.hpp"

typedef struct {
    int index;
    int64_t start_ms;
    int64_t end_ms;
    char text[1024];
} SubtitleEntry;
/**
 * 生成SRT格式字幕文件
 * @param videoPath 输入视频路径（用于获取时间信息）
 * @param subtitlePath 输出SRT文件路径
 * @param entries 字幕条目数组
 * @param count 字幕条目数量
 * @return 0成功，负数失败
 */
int generateSRT(const char* videoPath, const char* subtitlePath,
                SubtitleEntry* entries, int count);
#endif //FFMPEGLEARN_SRTGENERATOR_HPP
