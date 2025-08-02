//
// Created by 29051 on 2025/7/29.
//

#ifndef FFMPEGLEARN_TRANSCODE_HPP
#define FFMPEGLEARN_TRANSCODE_HPP
extern "C" {
    #include "libavfilter/avfilter.h"
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
    #include "libswscale/swscale.h"
    #include "libavutil/opt.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/error.h"
    #include "libavfilter/buffersink.h"
    #include "libavfilter/buffersrc.h"
}
#include "logging.hpp"
/**
 * 搞定
 * @param input_path
 * @param output_path
 * @return
 */
int transcode(const char *input_path, const char *output_path);
#endif //FFMPEGLEARN_TRANSCODE_HPP
