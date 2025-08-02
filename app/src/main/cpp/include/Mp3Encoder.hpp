//
// Created by 29051 on 2025/7/27.
//

#ifndef FFMPEGLEARN_MP3ENCODER_HPP
#define FFMPEGLEARN_MP3ENCODER_HPP
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include "lame.h"
#include "logging.hpp"

class Mp3Encoder {
private:
    std::optional<std::ifstream*> pcmFile;
    std::optional<std::ofstream*> mp3File;
    lame_t lameClient;
    const int sampleRate;
    const int channels;
    const int bitRate;
public:
    Mp3Encoder(const std::string &pcmPath, const std::string &mp3Path, int sampleRate, const int channels, const int bitRate);
    ~Mp3Encoder();
    void encode();
};

#endif //FFMPEGLEARN_MP3ENCODER_HPP
