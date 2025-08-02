//
// Created by 29051 on 2025/8/2.
//

#include "OpenGLLearn.hpp"

constexpr const char* TAG = "OpenGLLearn";

OpenGLLearn::OpenGLLearn(ANativeWindow *aNativeWindow) : display(EGL_NO_DISPLAY), surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT)  {
    this->aNativeWindow = aNativeWindow;
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display  == EGL_NO_DISPLAY){
        logger::error(TAG, "eglGetDisplay return error: %d, ", eglGetError());
        return;
    }
    if (!eglInitialize(display, nullptr, nullptr)){
        logger::error(TAG, "eglInitialize return error: %d, ", eglGetError());
        return;
    }
    const EGLint configAttribute[] = {
            EGL_BUFFER_SIZE, 32,
            EGL_ALPHA_SIZE,      8,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_DEPTH_SIZE,      16,
            EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribute, &config, 1, &numConfigs)){
        logger::error(TAG, "eglChooseConfig return error: %d, ", eglGetError());
        return;
    }
    const EGLint contextAttribute[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    context = eglCreateContext(display, config, nullptr, contextAttribute);
    if (!context){
        logger::error(TAG, "eglCreateContext return error: %d, ", eglGetError());
        return;
    }
    EGLint  format;
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)){
        logger::error(TAG, "eglGetConfigAttrib return error: %d, ", eglGetError());
        return;
    }

    ANativeWindow_setBuffersGeometry(aNativeWindow, 0, 0, format);
    surface = eglCreateWindowSurface(display, config, aNativeWindow, nullptr);
    if (!surface){
        logger::error(TAG, "eglCreateWindowSurface return error: %d, ", eglGetError());
        return;
    }

    eglMakeCurrent(display, surface, surface, context);
}

void OpenGLLearn::render() {
    int32_t width = ANativeWindow_getWidth(this->aNativeWindow);
    int32_t height = ANativeWindow_getHeight(this->aNativeWindow);
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // 红色背景
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display, surface);
}

OpenGLLearn::~OpenGLLearn() {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    logger::info(TAG, "destroy...");
}