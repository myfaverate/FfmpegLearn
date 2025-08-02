//
// Created by 29051 on 2025/8/2.
//

#ifndef FFMPEGLEARN_OPENSLLEARN_HPP
#define FFMPEGLEARN_OPENSLLEARN_HPP
extern "C" {
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "logging.hpp"

struct PlaybackContext {
    std::shared_ptr<std::ifstream> pcmFile;
    std::shared_ptr<std::vector<char>> buffer;
    SLObjectItf playerObject;
    SLObjectItf engineObject;
    SLObjectItf outputMixObject;
    std::unique_ptr<std::function<void(SLAndroidSimpleBufferQueueItf)>> callback;
};

void playPcm(const std::string &pcmPath);
#endif //FFMPEGLEARN_OPENSLLEARN_HPP
