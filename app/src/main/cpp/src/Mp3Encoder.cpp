//
// Created by 29051 on 2025/7/27.
//
#include "Mp3Encoder.hpp"
constexpr const char* TAG = "Mp3Encoder";
Mp3Encoder::Mp3Encoder(const std::string &pcmPath, const std::string &mp3Path, const int sampleRate,
                       const int channels, const int bitRate) : sampleRate(sampleRate),
                                                                channels(channels),
                                                                bitRate(bitRate) {
    this->pcmFile = std::make_optional(new std::ifstream(pcmPath, std::ios::binary));
    this->mp3File = std::make_optional(new std::ofstream(mp3Path, std::ios::binary));
    this->lameClient = lame_init();
    lame_set_in_samplerate(this->lameClient, sampleRate);
    lame_set_out_samplerate(this->lameClient, sampleRate);
    lame_set_num_channels(this->lameClient, channels);
    lame_set_brate(this->lameClient, bitRate / 1000);

    // 设置 id3v2 tag
    id3tag_set_title(this->lameClient, "zsh title");
    id3tag_set_artist(this->lameClient, "zsh artist");
    id3tag_set_album(this->lameClient, "zsh album");
    id3tag_set_year(this->lameClient, "2025");
    id3tag_set_comment(this->lameClient, "zsh 的 评论");
    // 专辑第几曲
    id3tag_set_track(this->lameClient, "1");
    // 流派
    id3tag_set_genre(this->lameClient, "Jazz");

    lame_init_params(this->lameClient);
    logger::info(TAG,
                 "Mp3Encoder init pcmPath: %s, mp3Path: %s, sampleRate: %d, channels: %d, bitRate: %d",
                 pcmPath.c_str(), mp3Path.c_str(), sampleRate, channels, bitRate);
}

Mp3Encoder::~Mp3Encoder() {
    logger::info(TAG, "~Mp3Encoder destroy...");
    this->pcmFile.and_then([](std::ifstream *pcmFile) -> std::optional<std::monostate> {
        if (pcmFile->is_open()) {
            pcmFile->close();
            delete pcmFile;
        }
        return std::monostate{}; // 必须返回 std::optional
    });
    this->pcmFile.reset();

    this->mp3File.and_then([](std::ofstream *mp3File) -> std::optional<std::monostate> {
        if (mp3File->is_open()) {
            mp3File->close();
            delete mp3File;
        }
        return std::monostate{}; // 必须返回 std::optional
    });
    this->mp3File.reset();

    lame_close(this->lameClient);
}

/**
 * int lame_encode_buffer(
    lame_t           gfp,           // LAME context
    const short int* buffer_l,     // 左声道 PCM
    const short int* buffer_r,     // 右声道 PCM
    const int        nsamples,     // 每个通道的样本数
    unsigned char*   mp3buf,       // 输出缓冲区
    const int        mp3buf_size   // 输出缓冲区大小
);
 */
void Mp3Encoder::encode() {

    // 假设 16-bit stereo PCM
    // 代表 pcm 1024 个 short数据 其以左右声道排序 LR LR ...
    constexpr int bufferSize = 1152 * 2; // 凑够一帧mp3数据

    std::vector<short> pcmBuffer(bufferSize);
    std::vector<short> leftBuffer(bufferSize / 2);
    std::vector<short> rightBuffer(bufferSize / 2);
    std::vector<unsigned char> mp3Buffer(bufferSize * 2);

    // buffer
    const auto pcm = this->pcmFile.value_or(nullptr);

    if (pcm == nullptr || !pcm->is_open()) {
        logger::info(TAG, "pcm not open...");
        return;
    }

    const auto mp3 = this->mp3File.value_or(nullptr);
    if (mp3 == nullptr || !mp3->is_open()) {
        logger::info(TAG, "mp3 not open...");
        return;
    }

    while (pcm->read(reinterpret_cast<char *>(pcmBuffer.data()), sizeof(short) * bufferSize)) {
        // char -> short -> channel
        const auto samples = pcm->gcount() / 2 / 2;
        for (int i = 0; i < samples; i++) {
            leftBuffer[i] = pcmBuffer[i << 1];
            rightBuffer[i] = pcmBuffer[(i << 1) + 1];
        }
        const int wroteSize = lame_encode_buffer(lameClient,
                                                 leftBuffer.data(),
                                                 rightBuffer.data(),
                                                 static_cast<const int>(samples), mp3Buffer.data(),
                                                 static_cast<int>(mp3Buffer.size()));
        mp3->write(reinterpret_cast<const char *>(mp3Buffer.data()), wroteSize);
        logger::info(TAG, "while sample: %d, wroteSize: %d", samples, wroteSize);
    }
    // 处理剩数据
    std::streamsize samples = pcm -> gcount() / 2 / 2;
    if (samples > 0) {
        const int wroteSize = lame_encode_buffer(lameClient,
                                                 leftBuffer.data(),
                                                 rightBuffer.data(),
                                                 static_cast<const int>(samples), mp3Buffer.data(),
                                                 static_cast<int>(mp3Buffer.size()));
        mp3->write(reinterpret_cast<const char *>(mp3Buffer.data()), wroteSize);
        const int flushSize = lame_encode_flush(lameClient, mp3Buffer.data(), static_cast<int>(mp3Buffer.size()));
        if (flushSize > 0){
            mp3->write(reinterpret_cast<const char *>(mp3Buffer.data()), flushSize);
        }
        logger::info(TAG, "out samples: %d, wroteSize: %d, flushSize: %d", samples, wroteSize, flushSize);
    }
    logger::info(TAG, "end...");
}