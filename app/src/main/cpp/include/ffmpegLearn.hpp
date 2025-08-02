//
// Created by 29051 on 2025/8/2.
//

#ifndef FFMPEGLEARN_FFMPEGLEARN_HPP
#define FFMPEGLEARN_FFMPEGLEARN_HPP

#include <jni.h>
#include <string>
#include <sstream>
#include <android/log.h>
#include <android/imagedecoder.h>
#include <format>
#include <sys/stat.h>
#include <exception>

#include "transcode.hpp"
#include "logging.hpp"
#include "lame.h"

#include "Mp3Encoder.hpp"
#include "FFmpegDecoder.hpp"
#include "OpenSLLearn.hpp"
#include "OpenGLLearn.hpp"

void list_protocols();

#endif //FFMPEGLEARN_FFMPEGLEARN_HPP
