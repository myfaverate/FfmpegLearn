# 编译 ffmpeg

### 官方配置
```shell
ffmpeg version 7.1.1-essentials_build-www.gyan.dev Copyright (c) 2000-2025 the FFmpeg developers
  built with gcc 14.2.0 (Rev1, Built by MSYS2 project)
  configuration: --enable-gpl --enable-version3 --enable-static --disable-w32threads --disable-autodetect --enable-fontconfig --enable-iconv --enable-gnutls --enable-libxml2 --enable-gmp --enable-bzlib --enable-lzma --enable-zlib --enable-libsrt --enable-libssh --enable-libzmq --enable-avisynth --enable-sdl2 --enable-libwebp --ena
ble-libx264 --enable-libx265 --enable-libxvid --enable-libaom --enable-libopenjpeg --enable-libvpx --enable-mediafoundation --enable-libass --enable-libfreetype --enable-libfribidi --enable-libharfbuzz --enable-libvidstab --enable-libvmaf --enable-libzimg --enable-amf --enable-cuda-llvm --enable-cuvid --enable-dxva2 --enable-d3d11
va --enable-d3d12va --enable-ffnvcodec --enable-libvpl --enable-nvdec --enable-nvenc --enable-vaapi --enable-libgme --enable-libopenmpt --enable-libopencore-amrwb --enable-libmp3lame --enable-libtheora --enable-libvo-amrwbenc --enable-libgsm --enable-libopencore-amrnb --enable-libopus --enable-libspeex --enable-libvorbis --enable-librubberband
```

### 准备工作 
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
# 开启 page size16k support
export LDFLAGS="-Wl,-z,max-page-size=16384"
```
**查看是否生效**
```shell
zsh@zsh:~/fribidi/android_build/arm64-v8a/lib$ readelf -l libfribidi.so | grep -A1 'LOAD'
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000017a1c 0x0000000000017a1c  R      0x4000
  LOAD           0x0000000000017a1c 0x000000000001ba1c 0x000000000001ba1c
                 0x0000000000004194 0x0000000000004194  R E    0x4000
  LOAD           0x000000000001bbb0 0x0000000000023bb0 0x0000000000023bb0
                 0x00000000000003f0 0x0000000000000450  RW     0x4000
  LOAD           0x000000000001bfa0 0x0000000000027fa0 0x0000000000027fa0
                 0x000000000000004c 0x0000000000000260  RW     0x4000
```

# armeabi-v7a arm32
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --target-os=android \
  --arch=arm \
  --cpu=armv7-a \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-everything \
  --enable-avcodec \
  --enable-avfilter \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-protocol=file \
  --disable-x86asm \
  --disable-static \
  --enable-shared \
  --prefix=./android_build/armeabi-v7a
make clean && make -j$(nproc) && make install

# 带音频字幕的编译命令
export ANDROID_NDK=/home/zsh/android-ndk-r27c

export RESULT=armeabi-v7a
#export RESULT=arm64-v8a
#export RESULT=x86_64
#export RESULT=x86

export LAME_PATH=/home/zsh/lame-3.100/android_build/${RESULT}
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig:/home/zsh/libass/android_build/${RESULT}/lib/pkgconfig:/home/zsh/zlib-1.3.1/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x264/android_build/${RESULT}/lib/pkgconfig:/home/zsh/lame-3.100/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x265_git/build/android_${RESULT}/android_build/${RESULT}/lib/pkgconfig
# 依赖验证
pkg-config --cflags libass
pkg-config --cflags harfbuzz
pkg-config --cflags libmp3lame
pkg-config --cflags freetype2
pkg-config --cflags fribidi
pkg-config --cflags x264
pkg-config --cflags x265

./configure \
  --target-os=android \
  --arch=arm \
  --cpu=armv7-a \
  --pkg-config=pkg-config \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=./android_build/${RESULT} \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --enable-lto \
  --enable-libfreetype \
  --enable-libharfbuzz \
  --enable-libass \
  --enable-libfribidi \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-demuxer=mov,matroska,mp3,aac \
  --enable-muxer=mp4,matroska,mp3,h264 \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-encoder=aac,libx264,libmp3lame \
  --enable-parser=h264,aac \
  --enable-filter=scale,overlay,drawtext,hue \
  --enable-bsf=aac_adtstoasc,h264_mp4toannexb \
  --enable-zlib \
  --enable-libass \
  --enable-libx264 \
  --enable-libx265 \
  --enable-jni \
  --enable-mediacodec \
  --enable-decoder=h264_mediacodec \
  --enable-decoder=hevc_mediacodec \
  --enable-encoder=h264_mediacodec \
  --enable-encoder=hevc_mediacodec \
  --enable-libmp3lame \
  --enable-gpl \
  --enable-demuxers \
  --enable-decoders \
  --enable-parsers \
  --enable-protocols \
  --disable-static \
  --enable-shared \
  --enable-small \
  --extra-cflags="-I$LAME_PATH/include" \
  --extra-ldflags="-L$LAME_PATH/lib -lmp3lame -lm" 
# 不知道pkg-config --cflags libmp3lame 为什么没有生效
make clean && make -j$(nproc) && make install
```

#  arm64-v8a arm64
strip: 移除调试符号
```shell
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/fribidi/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/lame-3.100/android_build/armeabi-v7a/lib/pkgconfig
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --target-os=android \
  --arch=aarch64 \
  --cpu=armv8-a \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --disable-everything \
  --enable-lto \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-protocol=file \
  --enable-demuxer=mov,matroska,mp3,aac \
  --enable-muxer=mp4,matroska,mp3 \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-parser=h264,aac \
  --enable-filter=scale,overlay,hue \
  --enable-bsf=aac_adtstoasc \
  --enable-encoder=aac \
  --enable-zlib \
  --disable-x86asm \
  --disable-static \
  --enable-shared \
  --prefix=./android_build/arm64-v8a


# 带音频字幕的编译命令
export ANDROID_NDK=/home/zsh/android-ndk-r27c

#export RESULT=armeabi-v7a
export RESULT=arm64-v8a
#export RESULT=x86_64
#export RESULT=x86

export LAME_PATH=/home/zsh/lame-3.100/android_build/${RESULT}
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig:/home/zsh/libass/android_build/${RESULT}/lib/pkgconfig:/home/zsh/zlib-1.3.1/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x264/android_build/${RESULT}/lib/pkgconfig:/home/zsh/lame-3.100/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x265_git/build/android_${RESULT}/android_build/${RESULT}/lib/pkgconfig
# 依赖验证
pkg-config --cflags libass
pkg-config --cflags libmp3lame
pkg-config --cflags harfbuzz
pkg-config --cflags freetype2
pkg-config --cflags fribidi
pkg-config --cflags x264
pkg-config --cflags x265

./configure \
  --target-os=android \
  --arch=aarch64 \
  --cpu=armv8-a \
  --pkg-config=pkg-config \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=./android_build/arm64-v8a \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --enable-lto \
  --enable-libfreetype \
  --enable-libharfbuzz \
  --enable-libass \
  --enable-libfribidi \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-muxer=mp4,matroska,mp3,h264 \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-encoder=aac,libx264,libmp3lame \
  --enable-parser=h264,aac \
  --enable-filter=scale,overlay,drawtext,hue \
  --enable-bsf=aac_adtstoasc,h264_mp4toannexb \
  --enable-zlib \
  --enable-libass \
  --enable-libx264 \
  --enable-libx265 \
  --enable-jni \
  --enable-mediacodec \
  --enable-decoder=h264_mediacodec \
  --enable-decoder=hevc_mediacodec \
  --enable-encoder=h264_mediacodec \
  --enable-encoder=hevc_mediacodec \
  --enable-libmp3lame \
  --enable-gpl \
  --enable-demuxers \
  --enable-decoders \
  --enable-parsers \
  --enable-protocols \
  --disable-x86asm \
  --disable-static \
  --enable-shared \
  --enable-small \
  --extra-cflags="-I$LAME_PATH/include" \
  --extra-ldflags="-L$LAME_PATH/lib -lmp3lame -lm" 
  
make clean && make -j$(nproc) && make install
```

# x86
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --target-os=android \
  --arch=x86 \
  --cpu=i686 \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-everything \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --disable-x86asm \
  --disable-asm \
  --disable-static \
  --enable-shared \
  --prefix=./android_build/x86
make clean && make -j$(nproc) && make install

# 静态库不需要加
--disable-asm

# 带音频字幕的编译命令 ===================================================
export ANDROID_NDK=/home/zsh/android-ndk-r27c

#export RESULT=armeabi-v7a
#export RESULT=arm64-v8a
#export RESULT=x86_64
export RESULT=x86

export LAME_PATH=/home/zsh/lame-3.100/android_build/${RESULT}
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig:/home/zsh/libass/android_build/${RESULT}/lib/pkgconfig:/home/zsh/zlib-1.3.1/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x264/android_build/${RESULT}/lib/pkgconfig:/home/zsh/lame-3.100/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x265_git/build/android_${RESULT}/android_build/${RESULT}/lib/pkgconfig
# 依赖验证
pkg-config --cflags libass
pkg-config --cflags harfbuzz
pkg-config --cflags libmp3lame
pkg-config --cflags freetype2
pkg-config --cflags fribidi
pkg-config --cflags x264
pkg-config --cflags x265

./configure \
  --target-os=android \
  --arch=x86 \
  --cpu=i686 \
  --pkg-config=pkg-config \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=./android_build/${RESULT} \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --enable-lto \
  --enable-libfreetype \
  --enable-libharfbuzz \
  --enable-libass \
  --enable-libfribidi \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-demuxers \
  --enable-decoders \
  --enable-parsers \
  --enable-protocols \
  --enable-muxer=mp4,matroska,mp3,h264 \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-encoder=aac,libx264,libmp3lame \
  --enable-parser=h264,aac \
  --enable-filter=scale,overlay,drawtext,hue \
  --enable-bsf=aac_adtstoasc,h264_mp4toannexb \
  --enable-zlib \
  --enable-libass \
  --enable-libx264 \
  --enable-libx265 \
  --enable-jni \
  --enable-mediacodec \
  --enable-decoder=h264_mediacodec \
  --enable-decoder=hevc_mediacodec \
  --enable-encoder=h264_mediacodec \
  --enable-encoder=hevc_mediacodec \
  --enable-libmp3lame \
  --enable-gpl \
  --disable-x86asm \
  --disable-asm \
  --disable-static \
  --enable-shared \
  --enable-small \
  --extra-cflags="-I$LAME_PATH/include" \
  --extra-ldflags="-L$LAME_PATH/lib -lmp3lame -lm" 
  
make clean && make -j$(nproc) && make install
```

# x86_64
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --target-os=android \
  --arch=x86_64 \
  --cpu=x86-64 \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-everything \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --disable-x86asm \
  --disable-static \
  --enable-shared \
  --prefix=./android_build/x86_64
make clean && make -j$(nproc) && make install


# 带音频字幕的编译命令
export ANDROID_NDK=/home/zsh/android-ndk-r27c

#export RESULT=armeabi-v7a
#export RESULT=arm64-v8a
export RESULT=x86_64
#export RESULT=x86

export LAME_PATH=/home/zsh/lame-3.100/android_build/${RESULT}
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig:/home/zsh/libass/android_build/${RESULT}/lib/pkgconfig:/home/zsh/zlib-1.3.1/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x264/android_build/${RESULT}/lib/pkgconfig:/home/zsh/lame-3.100/android_build/${RESULT}/lib/pkgconfig:/home/zsh/x265_git/build/android_${RESULT}/android_build/${RESULT}/lib/pkgconfig
# 依赖验证
pkg-config --cflags libass
pkg-config --cflags harfbuzz
pkg-config --cflags libmp3lame
pkg-config --cflags freetype2
pkg-config --cflags fribidi
pkg-config --cflags x264
pkg-config --cflags x265

./configure \
  --target-os=android \
  --arch=x86_64 \
  --cpu=x86-64 \
  --pkg-config=pkg-config \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=./android_build/${RESULT} \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --enable-lto \
  --enable-libfreetype \
  --enable-libharfbuzz \
  --enable-libass \
  --enable-libfribidi \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  --enable-muxer=mp4,matroska,mp3,h264 \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-encoder=aac,libx264,libmp3lame \
  --enable-parser=h264,aac \
  --enable-filter=scale,overlay,drawtext,hue \
  --enable-bsf=aac_adtstoasc,h264_mp4toannexb \
  --enable-zlib \
  --enable-libass \
  --enable-libx264 \
  --enable-libx265 \
  --enable-jni \
  --enable-mediacodec \
  --enable-decoder=h264_mediacodec \
  --enable-decoder=hevc_mediacodec \
  --enable-encoder=h264_mediacodec \
  --enable-encoder=hevc_mediacodec \
  --enable-libmp3lame \
  --enable-gpl \
  --disable-x86asm \
  --disable-static \
  --enable-shared \
  --enable-small \
  --enable-demuxers \
  --enable-decoders \
  --enable-parsers \
  --enable-protocols \
  --extra-cflags="-I$LAME_PATH/include" \
  --extra-ldflags="-L$LAME_PATH/lib -lmp3lame -lm" 
  
make clean && make -j$(nproc) && make install
```

## 快速复制
```shell
# armeabi-v7a arm32
export RESULT=armeabi-v7a
export RESULT=arm64-v8a
export RESULT=x86
export RESULT=x86_64
cp -R ~/lame-3.100/android_build/${RESULT}/lib/* ~/ffmpeg/android_build/${RESULT}/lib/* ~/x264/android_build/${RESULT}/lib/* ~/freetype/android_build/${RESULT}/lib/* ~/harfbuzz-main/android_build/${RESULT}/lib/* ~/fribidi/android_build/${RESULT}/lib/* /mnt/d/SoftWare/Android/AndroidProjects/KotlinAndroids/FfmpegLearn/app/src/main/jniLibs/${RESULT}/


cp ~/x265_git/build/android_${RESULT}/android_build/${RESULT}/lib/libx265.so /mnt/d/SoftWare/Android/AndroidProjects/KotlinAndroids/FfmpegLearn/app/src/main/jniLibs/${RESULT}/
```

## zlib
```shell
# 1 armeabi-v7a arm32
export TARGET=arm-linux-androideabi
# export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/fribidi/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/armeabi-v7a/lib/pkgconfig
export CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang
export CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++
export NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm
export LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld
export AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
export AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as
export RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
export STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip
./configure \
  --enable-shared \
  --prefix=$(pwd)/android_build/armeabi-v7a \
  --sharedlibdir=$(pwd)/android_build/armeabi-v7a/lib \
  --libdir=$(pwd)/android_build/armeabi-v7a/lib \
  --includedir=$(pwd)/android_build/armeabi-v7a/include
   
make clean && make -j$(nproc) && make install
# 2 其他架构
#export TARGET=aarch64-linux-android
#export TARGET=x86_64-linux-android
export TARGET=i686-linux-android

#export RESULT=arm64-v8a
#export RESULT=x86_64
export RESULT=x86

#export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig



export CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang
export CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang++
export NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm
export LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld
export AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
export AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as
export RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
export STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip
./configure \
  --enable-shared \
  --prefix=$(pwd)/android_build/${RESULT} \
  --sharedlibdir=$(pwd)/android_build/${RESULT}/lib \
  --libdir=$(pwd)/android_build/${RESULT}/lib \
  --includedir=$(pwd)/android_build/${RESULT}/include
  
make clean && make -j$(nproc) && make install
```


## 编译字幕

### freetype 编译

#### armeabi-v7a arm32
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --host=arm-linux-androideabi \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=/home/zsh/freetype/android_build/armeabi-v7a \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
make clean && make -j$(nproc) && make install
```
####  arm64-v8a arm64
```shell
export ANDROID_NDK=/home/zsh/android-ndk-r27c
./configure \
  --host=aarch64-linux-android \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=/home/zsh/freetype/android_build/arm64-v8a \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
make clean && make -j$(nproc) && make install
```

#### x86

```shell
./configure \
  --host=i686-linux-android \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=/home/zsh/freetype/android_build/x86 \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
make clean && make -j$(nproc) && make install
```

#### x86_64
```shell
./configure \
  --host=x86_64-linux-android \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=/home/zsh/freetype/android_build/x86_64 \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
make clean && make -j$(nproc) && make install

```

## fribidi
需要装c2man
**封装一下**

```shell
# armeabi-v7a arm32
export TARGET=arm-linux-androideabi
./configure \
  --host=${TARGET} \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/armeabi-v7a \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip
   
make clean && make -j$(nproc) && make install
# 2
export TARGET=aarch64-linux-android
#export TARGET=x86_64-linux-android
#export TARGET=i686-linux-android

export RESULT=arm64-v8a
#export RESULT=x86_64
#export RESULT=x86
./configure \
  --host=${TARGET} \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/${RESULT} \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
  
make clean && make -j$(nproc) && make install
```

### harfbuzz
#### armeabi-v7a arm32
**android_armv7a.txt**
```text
c = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang'
cpp = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++'
ar = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar'
strip = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip'
pkg-config = 'pkg-config'
ld = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/ld'
ranlib = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib'

[properties]
needs_exe_wrapper = true

[host_machine]
system = 'android'
cpu_family = 'arm'
cpu = 'armv7'
endian = 'little'
```
```shell
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/fribidi/android_build/armeabi-v7a/lib/pkgconfig
meson setup android_build_arm32 --cross-file android_armv7a.txt --prefix=$(pwd)/android_build/armeabi-v7a --default-library=shared
ninja clean && ninja && ninja install
```
#### arm64-v8a arm64
**android_arm64v8a.txt**
```text
[binaries]
c = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang'
cpp = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++'
ar = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar'
strip = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip'
pkg-config = 'pkg-config'
ld = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/ld'
ranlib = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib'

[properties]
needs_exe_wrapper = true

[host_machine]
system = 'android'
cpu_family = 'aarch64'
cpu = 'aarch64'
endian = 'little'
```
```shell
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/arm64-v8a/lib/pkgconfig:/home/zsh/fribidi/android_build/arm64-v8a/lib/pkgconfig
meson setup android_build_arm64 --cross-file android_arm64.txt --prefix=$(pwd)/android_build/arm64-v8a --default-library=shared
ninja clean && ninja && ninja install
```
#### x86
**android_x86.txt**
```shell
[binaries]
c = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang'
cpp = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android24-clang++'
ar = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar'
strip = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip'
pkg-config = 'pkg-config'
ld = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/ld'
ranlib = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib'

[properties]
needs_exe_wrapper = true

[host_machine]
system = 'android'
cpu_family = 'x86'
cpu = 'i686'
endian = 'little'
```
```shell
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/x86/lib/pkgconfig:/home/zsh/fribidi/android_build/x86/lib/pkgconfig
meson setup android_build_x86 --cross-file android_x86.txt --prefix=$(pwd)/android_build/x86 --default-library=shared
ninja clean && ninja && ninja install
```
#### x86_64
**android_x86_64.txt**
```text
[binaries]
c = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang'
cpp = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android24-clang++'
ar = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar'
strip = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip'
pkg-config = 'pkg-config'
ld = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/ld'
ranlib = '/home/zsh/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib'

[properties]
needs_exe_wrapper = true

[host_machine]
system = 'android'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
```
```shell
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/x86_64/lib/pkgconfig:/home/zsh/fribidi/android_build/x86_64/lib/pkgconfig
meson setup android_build_x86_64 --cross-file android_x86_64.txt --prefix=$(pwd)/android_build/x86_64 --default-library=shared
ninja clean && ninja && ninja install
```


### libass
```shell
# armeabi-v7a arm32
export TARGET=arm-linux-androideabi
export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/fribidi/android_build/armeabi-v7a/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/armeabi-v7a/lib/pkgconfig
./configure \
  --host=${TARGET} \
  --disable-static \
  --disable-require-system-font-provider \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/armeabi-v7a \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip
   
make clean && make -j$(nproc) && make install
# 2 其他架构
#export TARGET=aarch64-linux-android
#export TARGET=x86_64-linux-android
export TARGET=i686-linux-android

#export RESULT=arm64-v8a
#export RESULT=x86_64
export RESULT=x86

export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig

./configure \
  --host=${TARGET} \
  --disable-static \
  --disable-require-system-font-provider \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/${RESULT} \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip 
  
make clean && make -j$(nproc) && make install
```


--enable-libass
---

libass、fribidi、freetype

---

drawtext_filter
* libfreetype — 用于字体渲染
* libharfbuzz — 用于复杂文本排版（如中文、阿拉伯语）

---

## libx265
```shell
git clone https://bitbucket.org/multicoreware/x265_git.git
build_android_arm64.sh
```
#### arm64-v8a
```shell
#!/bin/bash

# 修改为你的实际 NDK 路径
NDK=/home/zsh/android-ndk-r27c
API=24
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64

cmake ../../source \
    -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-${API} \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_C_COMPILER=$TOOLCHAIN/bin/aarch64-linux-android${API}-clang \
    -DCMAKE_CXX_COMPILER=$TOOLCHAIN/bin/aarch64-linux-android${API}-clang++ \
    -DENABLE_PIC=ON \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DENABLE_CLI=OFF \
    -DEXPORT_C_API=ON \
    -DHIGH_BIT_DEPTH=OFF \
    -DENABLE_SHARED=ON \
    -DENABLE_ASSEMBLY=OFF \
    -DCMAKE_STRIP=$TOOLCHAIN/bin/llvm-strip \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/android_build/arm64-v8a \
    -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-z,max-page-size=16384" 

make clean && make -j$(nproc) && make install
```
**报错的话**
```shell
vim CMakeFiles/x265-shared.dir/link.txt
# 删除-lpthread
```

## libx264_encoder 

* https://code.videolan.org/videolan/x264.git
```shell
vim configure
else
        echo "SOSUFFIX=so" >> config.mak
        echo "API: $API"
        echo "SONAME=libx264.so" >> config.mak
        echo "SOFLAGS=-shared -Wl,-soname,\$(SONAME) $SOFLAGS" >> config.mak

zsh@zsh:~/x264/android_build/arm64-v8a/lib$ readelf -d libx264.so | grep SONAME
 0x000000000000000e (SONAME)             Library soname: [libx264.so.165]
# 查看
readelf -d libavcodec.so | grep NEEDED
```
```shell
# armeabi-v7a arm32
export TARGET=arm-linux-androideabi

export RESULT=armeabi-v7a

#export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig

export CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang
export CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++
export NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm
export LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld
export AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
export AS=$CC
export RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
#export STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip

./configure \
  --host=${TARGET} \
  --enable-shared \
  --enable-pic --disable-cli \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=$(pwd)/android_build/${RESULT}
   
make clean && make -j$(nproc) && make install

# 2 其他架构 armv7a-linux-androideabi24-clang
# armeabi-v7a arm32
#export TARGET=aarch64-linux-android
#export TARGET=x86_64-linux-android
export TARGET=i686-linux-android

#export RESULT=arm64-v8a
#export RESULT=x86_64
export RESULT=x86

#export PKG_CONFIG_PATH=/home/zsh/freetype/android_build/${RESULT}/lib/pkgconfig:/home/zsh/fribidi/android_build/${RESULT}/lib/pkgconfig:/home/zsh/harfbuzz-main/android_build/${RESULT}/lib/pkgconfig

export CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang
export CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang++
export NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm
export LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld
export AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
export AS=$CC
export RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib
export STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip

./configure \
  --host=${TARGET} \
  --enable-shared \
  --enable-pic --disable-cli \
  --disable-asm \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --prefix=$(pwd)/android_build/${RESULT}
  

  
make clean && make -j$(nproc) && make install
```

交叉编译 跳过 pkg-config
```shell
--extra-cflags="-I/path/to/include1 -I/path/to/include2" \
--extra-ldflags="-L/path/to/lib1 -L/path/to/lib2"
```

# 完整 命令 视频 音频 字幕
```shell
./configure \
  --target-os=android \
  --arch=aarch64 \
  --cpu=armv8-a \
  --enable-cross-compile \
  --cross-prefix=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android- \
  --cc=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang \
  --nm=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  --ar=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  --ranlib=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  --strip=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  \
  --prefix=./android_build/arm64-v8a \
  \
  --disable-doc \
  --disable-avdevice \
  --disable-network \
  --disable-programs \
  --disable-postproc \
  --disable-everything \
  \
  --enable-lto \
  --enable-avcodec \
  --enable-avformat \
  --enable-avutil \
  --enable-swscale \
  --enable-swresample \
  \
  --enable-protocol=file \
  --enable-demuxer=mov,matroska,mp3,aac \
  --enable-muxer=mp4,matroska,mp3 \
  \
  --enable-decoder=h264,aac,mp3,ass,subrip \
  --enable-encoder=aac,libx264 \
  --enable-parser=h264,aac \
  \
  --enable-filter=scale,overlay,drawtext,hue \
  --enable-bsf=aac_adtstoasc \
  \
  --enable-zlib \
  --enable-libass \
  \
  --disable-x86asm \
  --disable-static \
  --enable-shared
```

## Lame 编译
```shell
# armeabi-v7a arm32
export ANDROID_NDK=/home/zsh/android-ndk-r27c
export TARGET=arm-linux-androideabi
./configure \
  --host=${TARGET} \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/armeabi-v7a \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip \
  LDFLAGS="-Wl,-z,max-page-size=16384"
   
make clean && make -j$(nproc) && make install
# 在 lame-3.100/libmp3lame/util.c 添加 void lame_init_old(void) {}
# 2
export TARGET=aarch64-linux-android
#export TARGET=x86_64-linux-android
#export TARGET=i686-linux-android

export RESULT=arm64-v8a
#export RESULT=x86_64
#export RESULT=x86
./configure \
  --host=${TARGET} \
  --disable-static \
  --enable-shared \
  --with-sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
  --with-pic \
  --prefix=$(pwd)/android_build/${RESULT} \
  CC=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang \
  CXX=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/${TARGET}24-clang++ \
  NM=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-nm \
  LD=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/ld \
  AR=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar \
  AS=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-as \
  RANLIB=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ranlib \
  STRIP=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip  \
  LDFLAGS="-Wl,-z,max-page-size=16384"
make clean && make -j$(nproc) && make install
```
实现 pcm 转 mp3

## Windows 编译
```shell
./configure \
--arch=x86_64 \
--disable-doc \
--disable-avdevice \
--disable-network \
--disable-programs \
--disable-postproc \
--enable-gpl \
--enable-version3 \
--enable-nonfree \
--enable-cuda \
--enable-cuvid \
--enable-nvenc \
--enable-d3d11va \
--enable-dxva2 \
--enable-libx264 \
--enable-libx265 \
--enable-libvpx \
--enable-libopus \
--enable-libvorbis \
--enable-libwebp \
--enable-opencl \
--enable-libfreetype \
--enable-libdav1d \
--enable-lto \
--enable-avcodec \
--enable-avformat \
--enable-avutil \
--enable-swscale \
--enable-swresample \
--enable-protocol=file \
--enable-demuxer=mov,matroska,mp3,aac \
--enable-muxer=mp4,matroska,mp3 \
--enable-decoder=h264,aac,mp3,ass,subrip \
--enable-parser=h264,aac \
--enable-filter=scale,overlay,hue \
--enable-bsf=aac_adtstoasc \
--enable-encoder=aac \
--enable-zlib \
--disable-x86asm \
--disable-static \
--enable-shared \
--prefix=./windows_build

make clean && make -j$(nproc) && make install
```