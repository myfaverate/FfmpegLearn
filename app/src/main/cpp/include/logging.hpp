//
// Created by 29051 on 2025/7/5.
//

#ifndef WEBVIEWLEARN_LOGGING_HPP
#define WEBVIEWLEARN_LOGGING_HPP

#include <cstdarg>
#include <android/log.h>

// #define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,"webViewLearn",__VA_ARGS__) // 定义LOGI类型
namespace logger {
    inline void info(const char *tag, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, args);
        va_end(args);
    }
    inline void error(const char *tag, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, args);
        va_end(args);
    }
}
#endif //WEBVIEWLEARN_LOGGING_HPP
