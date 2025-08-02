//
// Created by 29051 on 2025/8/2.
//

#ifndef FFMPEGLEARN_OPENGLLEARN_HPP
#define FFMPEGLEARN_OPENGLLEARN_HPP

extern "C" {
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
}
#include "logging.hpp"

class OpenGLLearn{
public:
    OpenGLLearn(ANativeWindow *aNativeWindow);
    ~OpenGLLearn();
    void render();
private:
    ANativeWindow *aNativeWindow;
    void *display;
    EGLSurface surface;
    EGLContext context;
    GLuint VAO, VBO, EBO;
    GLuint textureId;
    unsigned int programId;
};

#endif //FFMPEGLEARN_OPENGLLEARN_HPP
