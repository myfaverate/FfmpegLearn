//
// Created by 29051 on 2025/8/2.
//
#include "OpenSLLearn.hpp"
constexpr const char *TAG = "OpenSLLearn";

// 桥接函数，OpenSL 会调用它
void bufferCallback(SLAndroidSimpleBufferQueueItf queue, void* context) {
    auto *ctx = static_cast<PlaybackContext*>(context);
    auto &pcmFile = ctx->pcmFile;
    auto &buffer = ctx->buffer;
    pcmFile->read(buffer->data(), static_cast<std::streamsize>(buffer->size()));
    auto readSize = pcmFile->gcount();
    if (readSize > 0) {
        SLresult result = (*queue)->Enqueue(queue, buffer->data(), readSize);
        if (result != SL_RESULT_SUCCESS){
            logger::error(TAG, "Enqueue failed: %d", result);
        }
    } else {
        logger::info(TAG, "播放结束...");
        (*ctx->playerObject)->Destroy(ctx->playerObject);
        (*ctx->outputMixObject)->Destroy(ctx->outputMixObject);
        (*ctx->engineObject)->Destroy(ctx->engineObject);
        delete ctx;  // 一次释放所有资源
    }
}

void playPcm(const std::string &pcmPath){

    logger::info(TAG, "pcmPath: %s", pcmPath.c_str());

    SLObjectItf engineObject;
    SLEngineItf engineEngine;

    slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);


    SLObjectItf outputMixObject;
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, nullptr, nullptr);
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);

    SLDataLocator_AndroidBufferQueue locatorAndroidBufferQueue = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource audioSrc = { &locatorAndroidBufferQueue, &formatPcm };

    SLDataLocator_OutputMix locatorOutputMix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
    SLDataSink audioSink = { &locatorOutputMix, nullptr };

    const SLInterfaceID  ids[1] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
    const SLboolean  req[1] = { SL_BOOLEAN_TRUE };


    SLObjectItf playerObject = nullptr;
    SLPlayItf playerPlay = nullptr;
    SLAndroidSimpleBufferQueueItf bufferQueue = nullptr;

    (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc, &audioSink, 1, ids, req);
    (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    (*playerObject)->GetInterface(playerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &bufferQueue);

    // std::ifstream pcmFile(pcmPath, std::ios::binary);
    // 创建共享资源
    auto pcmFile = std::make_unique<std::ifstream>(pcmPath, std::ios::binary);

    if (!pcmFile->is_open()){
        throw std::runtime_error("文件打开错误...");
    }

    // std::vector<char> buffer(4096);
    auto buffer = std::make_unique<std::vector<char>>(4096);

    auto *playBackContext = new PlaybackContext{
        std::move(pcmFile),
        std::move(buffer),
        playerObject,
        engineObject,
        outputMixObject,
    };

    (*bufferQueue)->RegisterCallback(bufferQueue, bufferCallback, static_cast<void*>(playBackContext));

    (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    bufferCallback(bufferQueue, playBackContext);

}
