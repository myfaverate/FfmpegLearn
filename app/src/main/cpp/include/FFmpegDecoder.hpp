//
// Created by 29051 on 2025/7/28.
//

#ifndef FFMPEGLEARN_FFMPEGDECODER_HPP
#define FFMPEGLEARN_FFMPEGDECODER_HPP

#include <string>
#include <fstream>
#include <vector>

extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavformat/avformat.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include <libswscale/swscale.h>
}

#include "logging.hpp"

/**
 * 从MP4当中分离出pcm和yuv裸数据
 * @param videoPath
 */
void extractYumPcmFromMp4(const std::string &videoPath);

#endif //FFMPEGLEARN_FFMPEGDECODER_HPP
