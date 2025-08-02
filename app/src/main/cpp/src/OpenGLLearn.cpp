//
// Created by 29051 on 2025/8/2.
//

#include "OpenGLLearn.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void checkShaderCompile(GLuint shader, const char* type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        logger::error(TAG, "%s shader compile error: %s", type, infoLog);
    }
}
void checkProgramLink(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        logger::error(TAG, "Shader program link error: %s", infoLog);
    }
}

void OpenGLLearn::render() {
    int32_t width = ANativeWindow_getWidth(this->aNativeWindow);
    int32_t height = ANativeWindow_getHeight(this->aNativeWindow);
    glViewport(0, 0, width, height);

    // 1. Hello World
    // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // 红色背景
    // glClear(GL_COLOR_BUFFER_BIT);
    // eglSwapBuffers(display, surface);

    // 2. load png
    int imageWidth, imageHeight, imageChannels;
    auto imageData = stbi_load("/sdcard/Pictures/image_1753108849953.jpg", &imageWidth, &imageHeight, &imageChannels, STBI_rgb_alpha);
    if (!imageData) {
        logger::error(TAG, "Failed to load image! imageData: %s", stbi_failure_reason());
        return;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 加载纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData); // 用完就释放


    float imageAspect = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    float screenAspect = static_cast<float>(width) / static_cast<float >(height);

    float quadWidth = 1.0f;
    float quadHeight = 1.0f;

// 按照宽度铺满，高度居中裁剪或留空
    if (imageAspect > screenAspect) {
        quadHeight = screenAspect / imageAspect;
    } else {
        quadWidth = imageAspect / screenAspect;
    }

    float vertices[] = {
            // 位置                // 纹理坐标
            -quadWidth,  quadHeight,  0.0f, 0.0f,  // 左上
            -quadWidth, -quadHeight,  0.0f, 1.0f,  // 左下
            quadWidth, -quadHeight,  1.0f, 1.0f,  // 右下
            quadWidth,  quadHeight,  1.0f, 0.0f   // 右上
    };
    unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// 顶点位置
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
// 纹理坐标
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    const char *vertexShaderSource = R"(#version 300 es
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos.xy, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char *fragmentShaderSource = R"(#version 300 es
        precision mediump float;
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    checkShaderCompile(vertexShader, "Vertex");
    checkShaderCompile(fragmentShader, "Fragment");
    checkProgramLink(programId);



// 使用程序
    glUseProgram(programId);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    eglSwapBuffers(display, surface);

}

OpenGLLearn::~OpenGLLearn() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureId);
    glDeleteProgram(programId);

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    ANativeWindow_release(aNativeWindow);
    logger::info(TAG, "destroy...");
}