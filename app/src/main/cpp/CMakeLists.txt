
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("ffmpegLearn")

# ====================================== 测试 ===========================================
message("sourceDir: ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("ANDROID_ABI: ${ANDROID_ABI}")
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

# 设置库文件基础路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

# ffmpeg 头文件路径
# include_directories(${CMAKE_SOURCE_DIR}/ffmpeg)
# 引入 ffmpeg 库（动态库）
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavcodec.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libavutil.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libswresample.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libswscale.so)

# x264 头文件路径
# include_directories(${CMAKE_SOURCE_DIR}/x264)
message(${CMAKE_SOURCE_DIR}/x264)
# 引入 x264 库（动态库）
add_library(x264 SHARED IMPORTED)
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libx264.so)
message(${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libx264.so)

# lame
add_library(mp3lame SHARED IMPORTED)
set_target_properties(mp3lame PROPERTIES IMPORTED_LOCATION ${LIBRARY_OUTPUT_PATH}/libmp3lame.so)
# =================================== 测试 ================================================

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.

# 查找源文件
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
)
message("SOURCES: ${SOURCES}")
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")

add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        ffmpegLearn.cpp ${SOURCES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE __STDC_CONSTANT_MACROS)

# 使用 target_include_directories 替代 include_directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/ffmpeg
        ${CMAKE_SOURCE_DIR}/x264
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/std
)

## 设置头文件包含路径
#message("PROJECT_NAME: ${PROJECT_NAME}")
#message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
#target_include_directories(${CMAKE_PROJECT_NAME}
#        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
#)
## 链接指定路径的共享库
#target_link_libraries(${PROJECT_NAME}
#        PUBLIC
#        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libmp3lame.so
#)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        x264
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        mp3lame
        # List libraries link to the target library
        android
        log
        OpenSLES
        GLESv3
        EGL
)