//
// Created by 29051 on 2025/8/2.
//
#include "OpenSLLearn.hpp"
constexpr const char *TAG = "OpenSLLearn";

// 桥接函数，OpenSL 会调用它
void bufferCallback(SLAndroidSimpleBufferQueueItf queue, void* context) {
    auto* func = static_cast<std::function<void(SLAndroidSimpleBufferQueueItf)>*>(context);
    if (func && *func) {
        (*func)(queue);
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
    auto pcmFile = std::make_shared<std::ifstream>(pcmPath, std::ios::binary);

    if (!pcmFile->is_open()){
        throw std::runtime_error("文件打开错误...");
    }

    // std::vector<char> buffer(4096);
    auto buffer = std::make_shared<std::vector<char>>(4096);


    // 创建 lambda 并用 std::function 包裹
    // 为了防止 lambda 被释放，持有它
    static auto staticHoldBuffer = buffer;
    static auto staticHoldFile = pcmFile;
    auto callback = new std::function<void(SLAndroidSimpleBufferQueueItf)>();
    *callback = [=](SLAndroidSimpleBufferQueueItf queue) {
        if (!pcmFile->is_open()) return;

        if (pcmFile->eof()) {
            logger::info(TAG, "PCM 播放完毕");
            pcmFile->close();
            staticHoldBuffer.reset();
            staticHoldFile.reset();
            delete callback;
            return;
        }
        pcmFile->read(buffer->data(), static_cast<std::streamsize>(buffer->size()));
        std::streamsize read = pcmFile->gcount();
        logger::info(TAG, "readSize: %d", read);
        if (read > 0) {
            SLresult result = (*queue)->Enqueue(queue, buffer->data(), read);
            if (result != SL_RESULT_SUCCESS){
                logger::error(TAG, "Enqueue failed: %d", result);
            }
        }
    };


    (*bufferQueue)->RegisterCallback(bufferQueue, bufferCallback, callback);

    (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    bufferCallback(bufferQueue, callback);

}
