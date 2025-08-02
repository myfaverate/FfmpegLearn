//
// Created by 29051 on 2025/5/18.
//


#include "transcode.hpp"

constexpr const char* TAG = "transcode";


static void print_error(const char *msg, int err) {
    char buf[256];
    av_strerror(err, buf, sizeof(buf));
    fprintf(stderr, "%s: %s\n", msg, buf);
}

/**
 * 搞定
 * @param input_path
 * @param output_path
 * @return
 */
int transcode(const char *input_path, const char *output_path) {

    logger::info(TAG, "AVCodec version: %d\n", avcodec_version());
// 测试H264解码器可用性
    auto codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        logger::error(TAG, "H264解码器不可用");
    } else {
        logger::info(TAG, "H264解码器可用");
    }

    int ret = 0;

    AVFormatContext *ifmt_ctx = nullptr;
    AVFormatContext *ofmt_ctx = nullptr;
    AVCodecContext *dec_ctx = nullptr;
    AVCodecContext *enc_ctx = nullptr;
    AVStream *in_video_stream = nullptr;
    AVStream *out_video_stream = nullptr;
    struct SwsContext *sws_ctx = nullptr;

    int video_stream_index = -1;

    AVPacket *pkt = nullptr;
    AVFrame *frame = nullptr;
    AVFrame *enc_frame = nullptr;

    // 1. 打开输入文件
    if ((ret = avformat_open_input(&ifmt_ctx, input_path, nullptr, nullptr)) < 0) {
        logger::info(TAG, "Cannot open input file error: %s", av_err2str(ret));
        return -1;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, nullptr)) < 0) {
        logger::info(TAG, "Cannot find stream info error: %s", av_err2str(ret));
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        return -1;
    }

    // 3. 创建输出上下文
    avformat_alloc_output_context2(&ofmt_ctx, nullptr, nullptr, output_path);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = -1;
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return ret;
    }

    int audio_stream_index = -1;
    AVStream* in_audio_stream = nullptr;
    AVStream* out_audio_stream = nullptr;

    // 找视频流 和 音频流
    for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = (int)i;
        } else if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = static_cast<int>(i);
            // in_audio_stream = ifmt_ctx->streams[i];
            //
            // // 在输出文件中创建一个对应音频流
            // out_audio_stream = avformat_new_stream(ofmt_ctx, nullptr);
            // if (!out_audio_stream) {
            //     LOGW("Failed to create output audio stream");
            //     return -1;
            // }
            //
            // // 直接复制参数
            // avcodec_parameters_copy(out_audio_stream->codecpar, in_audio_stream->codecpar);
            // out_audio_stream->codecpar->codec_tag = 0;
            // out_audio_stream->time_base = in_audio_stream->time_base;
            // 在创建输出音频流时增加校验
            // 在创建输出音频流时增加全面校验
            if (audio_stream_index >= 0) {
                in_audio_stream = ifmt_ctx->streams[audio_stream_index];
                out_audio_stream = avformat_new_stream(ofmt_ctx, nullptr);

                if (!out_audio_stream || !in_audio_stream->codecpar) {
                    logger::error(TAG, "Failed to setup audio stream");
                    audio_stream_index = -1;
                } else {
                    // 深度复制音频参数
                    if (avcodec_parameters_copy(out_audio_stream->codecpar, in_audio_stream->codecpar) < 0) {
                        logger::error(TAG, "Failed to copy audio codec parameters");
                        audio_stream_index = -1;
                    } else {
                        // 关键：同步输入输出流的时间基
                        out_audio_stream->time_base = in_audio_stream->time_base;
                        logger::info(TAG, "Audio stream created: codec=%d, time_base=%d/%d",
                             out_audio_stream->codecpar->codec_id,
                             out_audio_stream->time_base.num,
                             out_audio_stream->time_base.den);
                    }
                }
            }
            break;
        }
    }
    if (video_stream_index == -1) {
        fprintf(stderr, "No video stream found\n");
        ret = -1;
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        return ret;
    }

    in_video_stream = ifmt_ctx->streams[video_stream_index];

    // 2. 找解码器并打开
    const AVCodec* decoder = avcodec_find_decoder(in_video_stream->codecpar->codec_id);
    if (!decoder) {
        fprintf(stderr, "Decoder not found\n");
        ret = -1;
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        return ret;
    }

    dec_ctx = avcodec_alloc_context3(decoder);
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate decoder context\n");
        ret = -1;
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        return ret;
    }

    if ((ret = avcodec_parameters_to_context(dec_ctx, in_video_stream->codecpar)) < 0) {
        print_error("Failed to copy codec params to decoder context", ret);
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        return -1;
    }

    if ((ret = avcodec_open2(dec_ctx, decoder, nullptr)) < 0) {
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        print_error("Failed to open decoder", ret);
        return -1;
    }


    // 4. 找编码器
    const AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!encoder) {
        fprintf(stderr, "Encoder not found\n");
        ret = -1;
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return ret;
    }

    // 5. 新建视频流
    out_video_stream = avformat_new_stream(ofmt_ctx, nullptr);
    if (!out_video_stream) {
        fprintf(stderr, "Failed allocating output stream\n");
        ret = -1;
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return ret;
    }

    enc_ctx = avcodec_alloc_context3(encoder);
    if (!enc_ctx) {
        fprintf(stderr, "Failed to allocate encoder context\n");
        ret = -1;
        avcodec_free_context(&dec_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return ret;
    }

    // 6. 配置编码器参数，和目标分辨率、格式对应
    enc_ctx->height = 720;
    enc_ctx->width = 1280;
    enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
    // 使用 baseline profile
    av_opt_set(enc_ctx->priv_data, "profile", "baseline", 0);
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->time_base = av_inv_q(dec_ctx->framerate);
    if (enc_ctx->time_base.num == 0 || enc_ctx->time_base.den == 0) {
        enc_ctx->time_base = dec_ctx->time_base;
    }
    // 编码器一些参数(可根据需求调整)
    enc_ctx->bit_rate = 2 * 1000 * 1000; // 2Mbps

    // 设置 GOP 大小（关键帧间隔）
    enc_ctx->gop_size = 12;
    enc_ctx->max_b_frames = 0;

    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    // 7. 打开编码器
    if ((ret = avcodec_open2(enc_ctx, encoder, nullptr)) < 0) {
        print_error("Cannot open encoder", ret);
        avcodec_free_context(&dec_ctx);
        avcodec_free_context(&enc_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return -1;
    }

    ret = avcodec_parameters_from_context(out_video_stream->codecpar, enc_ctx);
    out_video_stream->time_base = enc_ctx->time_base;
    if (ret < 0) {
        print_error("Failed to copy encoder params to output stream", ret);
        avcodec_free_context(&dec_ctx);
        avcodec_free_context(&enc_ctx);
        if (ifmt_ctx)
            avformat_close_input(&ifmt_ctx);
        if (ofmt_ctx) {
            if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
        }
        return -1;
    }

    // out_video_stream->time_base = enc_ctx->time_base;

    // 8. 打开输出文件
    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, output_path, AVIO_FLAG_WRITE);
        if (ret < 0) {
            print_error("Could not open output file", ret);
            ofmt_ctx->pb = nullptr;  // 显式置空，避免清理时访问无效指针
            avcodec_free_context(&dec_ctx);
            avcodec_free_context(&enc_ctx);
            if (ifmt_ctx)
                avformat_close_input(&ifmt_ctx);
            if (ofmt_ctx) {
                if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
                    avio_closep(&ofmt_ctx->pb);
                avformat_free_context(ofmt_ctx);
            }
            return -1;
        }
    }

    // 设置 movflags +faststart
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "movflags", "faststart", 0);


    av_dump_format(ofmt_ctx, 0, output_path, 1);

    int64_t frameIndex = 0;

    // 写文件头
    if ((ret = avformat_write_header(ofmt_ctx, &opts)) < 0) {
        print_error("Error occurred when opening output file", ret);
        goto end;
    }
    av_dict_free(&opts);

    // 9. 分配包和帧
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate packet\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    frame = av_frame_alloc();
    enc_frame = av_frame_alloc();
    if (!frame || !enc_frame) {
        fprintf(stderr, "Could not allocate frames\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // 初始化编码帧
    enc_frame->format = enc_ctx->pix_fmt;
    enc_frame->width = enc_ctx->width;
    enc_frame->height = enc_ctx->height;
    if ((ret = av_frame_get_buffer(enc_frame, 32)) < 0) {
        print_error("Could not allocate encoding frame buffer", ret);
        goto end;
    }

    // 10. 创建sws上下文，用于缩放和像素格式转换
    sws_ctx = sws_getContext(
            dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
            enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt,
            SWS_LANCZOS, nullptr, nullptr, nullptr
    );



    if (!sws_ctx) {
        fprintf(stderr, "Could not initialize the conversion context\n");
        ret = -1;
        goto end;
    }



    // 11. 读取帧、解码、缩放、编码循环
    while (av_read_frame(ifmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            ret = avcodec_send_packet(dec_ctx, pkt);
            if (ret < 0) {
                print_error("Error sending packet to decoder", ret);
                av_packet_unref(pkt);
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    print_error("Error during decoding", ret);
                    goto end;
                }

                // 转换像素格式和尺寸
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize,
                          0, dec_ctx->height,
                          enc_frame->data,
                          enc_frame->linesize);

                if (frame->pts == AV_NOPTS_VALUE) {
                    enc_frame->pts = frameIndex ++ ;
                } else {
                    int64_t pts = av_rescale_q(frame->pts, in_video_stream->time_base, enc_ctx->time_base);
                    enc_frame->pts = pts;
                }


                // 发送帧给编码器
                ret = avcodec_send_frame(enc_ctx, enc_frame);
                if (ret < 0) {
                    print_error("Error sending frame to encoder", ret);
                    goto end;
                }

                // 从编码器取包写文件
                while (true) {
                    ret = avcodec_receive_packet(enc_ctx, pkt);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    else if (ret < 0) {
                        print_error("Error during encoding", ret);
                        goto end;
                    }

                    pkt->stream_index = out_video_stream->index;
                    av_packet_rescale_ts(pkt, enc_ctx->time_base, out_video_stream->time_base);

                    ret = av_interleaved_write_frame(ofmt_ctx, pkt);
                    av_packet_unref(pkt);
                    if (ret < 0) {
                        print_error("Error muxing packet", ret);
                        goto end;
                    }
                }
            }
        } else if (audio_stream_index >= 0 && pkt->stream_index == audio_stream_index) {
            // 音频处理逻辑
            AVPacket* audio_pkt = av_packet_clone(pkt); // 避免修改原数据包
            if (!audio_pkt) continue;
            if (in_audio_stream != nullptr && out_audio_stream != nullptr) {
                // 时间戳转换
                audio_pkt->pts = av_rescale_q(audio_pkt->pts,
                                              in_audio_stream->time_base,
                                              out_audio_stream->time_base);
                audio_pkt->stream_index = out_audio_stream->index;
            }
            if (av_interleaved_write_frame(ofmt_ctx, audio_pkt) < 0) {
                logger::error(TAG, "Failed to write audio frame");
            }
            av_packet_free(&audio_pkt);
        }
        av_packet_unref(pkt);
    }

    logger::info(TAG, "Input audio stream exists: %s",
         audio_stream_index >= 0 ? "YES" : "NO");
    if (out_audio_stream) {
        logger::info(TAG, "Output audio codec: %d, timebase: %d/%d",
             out_audio_stream->codecpar->codec_id,
             out_audio_stream->time_base.num,
             out_audio_stream->time_base.den);
    }
    logger::info(TAG, "Writing audio packet: pts=%ld, size=%d",
         pkt->pts, pkt->size);

    for (unsigned int i = 0; i < ofmt_ctx->nb_streams; ++i) {
        AVStream* stream = ofmt_ctx->streams[i];
        logger::info(TAG, "Output stream %d: codec_id=%d, codec_type=%d, time_base=%d/%d",
             i,
             stream->codecpar->codec_id,
             stream->codecpar->codec_type,
             stream->time_base.num,
             stream->time_base.den);
    }

    // 在访问音频流前添加检查
    if (in_audio_stream && in_audio_stream->codecpar) {
        logger::info(TAG, "Audio codec_id: %d", in_audio_stream->codecpar->codec_id);
    } else {
        logger::info(TAG, "No audio stream or invalid codecpar");
    }

    // 12. 发送空帧刷新编码器
    avcodec_send_frame(enc_ctx, nullptr);
    while (true) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            break;
        else if (ret < 0) {
            print_error("Error during flushing encoder", ret);
            goto end;
        }

        pkt->stream_index = out_video_stream->index;
        av_packet_rescale_ts(pkt, enc_ctx->time_base, out_video_stream->time_base);
        ret = av_interleaved_write_frame(ofmt_ctx, pkt);
        av_packet_unref(pkt);
        if (ret < 0) {
            print_error("Error muxing packet", ret);
            goto end;
        }
    }

    // 13. 写文件尾
    av_write_trailer(ofmt_ctx);

end:
    if (sws_ctx)
        sws_freeContext(sws_ctx);
    avcodec_free_context(&dec_ctx);
    avcodec_free_context(&enc_ctx);
    if (ifmt_ctx)
        avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx) {
        if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
    }
    av_frame_free(&frame);
    av_frame_free(&enc_frame);
    av_packet_free(&pkt);

    return ret;
}


/**
 * 搞定
 * @param input_path
 * @param output_path
 * @return
 */
int getSubtitle(const char *inputPath, const char *outputPath) {
    int ret = 0;
    AVFormatContext *iAVFormatContext = nullptr;
    AVFormatContext *oAVFormatContext = nullptr;

    // 1. 打开输入文件
    if ((ret = avformat_open_input(&iAVFormatContext, inputPath, nullptr, nullptr)) < 0) {
        print_error("Cannot open input file", ret);
        return -1;
    }

    if ((ret = avformat_find_stream_info(iAVFormatContext, nullptr)) < 0) {
        print_error("Cannot find stream info", ret);
        avformat_close_input(&iAVFormatContext);
        return -1;
    }

    // 2. 查找字幕流
    int subtitle_stream_index = -1;
    AVStream* in_subtitle_stream = nullptr;
    for (unsigned int i = 0; i < iAVFormatContext->nb_streams; i++) {
        AVCodecParameters *codecpar = iAVFormatContext->streams[i]->codecpar;
        if (codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            subtitle_stream_index = i;
            in_subtitle_stream = iAVFormatContext->streams[i];
            logger::info(TAG, "Found subtitle stream #%d (codec: %s)",
                 i, avcodec_get_name(codecpar->codec_id));
            break;
        }
    }

    if (subtitle_stream_index == -1) {
        logger::error(TAG, "No subtitle stream found");
        avformat_close_input(&iAVFormatContext);
        return -1;
    }

    // 3. 创建输出上下文（SRT格式）
    avformat_alloc_output_context2(&oAVFormatContext, nullptr, "srt", outputPath);
    if (!oAVFormatContext) {
        logger::error(TAG, "Could not create output context");
        avformat_close_input(&iAVFormatContext);
        return -1;
    }

    // 4. 初始化数据包
    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        logger::error(TAG, "Could not allocate packet");
        avformat_close_input(&iAVFormatContext);
        avformat_free_context(oAVFormatContext);
        return -1;
    }

    // 5. 打开输出文件
    if (!(oAVFormatContext->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&oAVFormatContext->pb, outputPath, AVIO_FLAG_WRITE)) < 0) {
            print_error("Could not open output file", ret);
            av_packet_free(&pkt);
            avformat_close_input(&iAVFormatContext);
            avformat_free_context(oAVFormatContext);
            return -1;
        }
    }

    // 6. 写入文件头
    if ((ret = avformat_write_header(oAVFormatContext, nullptr)) < 0) {
        print_error("Error writing header", ret);
        av_packet_free(&pkt);
        avformat_close_input(&iAVFormatContext);
        if (!(oAVFormatContext->oformat->flags & AVFMT_NOFILE))
            avio_closep(&oAVFormatContext->pb);
        avformat_free_context(oAVFormatContext);
        return -1;
    }

    // 7. 提取字幕数据
    while (av_read_frame(iAVFormatContext, pkt) >= 0) {
        if (pkt->stream_index == subtitle_stream_index) {
            // 直接写入原始字幕数据（适用于文本字幕如SRT）
            if (av_write_frame(oAVFormatContext, pkt) < 0) {
                logger::error(TAG, "Failed to write subtitle packet");
            }
        }
        av_packet_unref(pkt);
    }

    // 8. 写入文件尾
    av_write_trailer(oAVFormatContext);

    // 9. 资源清理
    av_packet_free(&pkt);
    avformat_close_input(&iAVFormatContext);
    if (!(oAVFormatContext->oformat->flags & AVFMT_NOFILE))
        avio_closep(&oAVFormatContext->pb);
    avformat_free_context(oAVFormatContext);

    logger::info(TAG, "Subtitle extracted successfully to: %s", outputPath);
    return 0;
}

/**
 * 硬字幕 有 bug
 * TODO Cannot create subtitles filter: Out of memory
 * @return
 */
/**
 * 硬字幕烧录实现
 * @param inputPath 输入视频路径
 * @param subtitlePath 字幕文件路径
 * @param outputPath 输出视频路径
 * @return 0成功，负数失败
 */
int burnSubtitles(const char* inputPath, const char* subtitlePath, const char* outputPath) {
    // 初始化变量
    AVFormatContext *in_fmt_ctx = NULL, *out_fmt_ctx = NULL;
    AVCodecContext *dec_ctx = NULL, *enc_ctx = NULL;
    AVFilterGraph *filter_graph = NULL;
    AVFilterContext *buffersrc_ctx = NULL, *buffersink_ctx = NULL;
    AVStream *in_stream = NULL, *out_stream = NULL;
    const AVCodec *decoder = NULL, *encoder = NULL;
    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;
    int video_stream_index = -1;
    int ret = 0;

    av_max_alloc(128 * 1024 * 1024);
    // 2. 打开输入文件（增加超时设置）
    AVDictionary *options = NULL;
    av_dict_set(&options, "stimeout", "5000000", 0); // 5秒超时

    // 1. 打开输入文件
    if ((ret = avformat_open_input(&in_fmt_ctx, inputPath, NULL, NULL)) < 0) {
        logger::error(TAG, "Could not open input file: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    if ((ret = avformat_find_stream_info(in_fmt_ctx, NULL)) < 0) {
        logger::error(TAG, "Failed to find stream information: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 2. 查找视频流
    video_stream_index = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (video_stream_index < 0) {
        logger::error(TAG, "Could not find video stream");
        ret = -1;
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 3. 初始化解码器
    dec_ctx = avcodec_alloc_context3(decoder);
    if (!dec_ctx) {
        logger::error(TAG, "Failed to allocate decoder context");
        ret = AVERROR(ENOMEM);
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    in_stream = in_fmt_ctx->streams[video_stream_index];
    if ((ret = avcodec_parameters_to_context(dec_ctx, in_stream->codecpar)) < 0) {
        logger::error(TAG, "Failed to copy decoder parameters: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    if ((ret = avcodec_open2(dec_ctx, decoder, NULL)) < 0) {
        logger::error(TAG, "Failed to open decoder: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 4. 创建输出上下文
    avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, outputPath);
    if (!out_fmt_ctx) {
        logger::error(TAG, "Could not create output context");
        ret = AVERROR(ENOMEM);
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 5. 初始化编码器
    encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!encoder) {
        logger::error(TAG, "H.264 encoder not found");
        ret = -1;
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    enc_ctx = avcodec_alloc_context3(encoder);
    if (!enc_ctx) {
        logger::error(TAG, "Failed to allocate encoder context");
        ret = AVERROR(ENOMEM);
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 配置编码参数
// 修改编码器配置部分
    enc_ctx->height = dec_ctx->height;
    enc_ctx->width = dec_ctx->width;
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->time_base = in_stream->time_base;

// 关键修改：降低Level限制
    enc_ctx->level = 42;  // 改为Level 4.2，适合1080p视频
    enc_ctx->bit_rate = 2000000;
    enc_ctx->gop_size = 12;
    enc_ctx->max_b_frames = 2;  // 减少B帧数量

// 设置合理的视频尺寸限制
    if (enc_ctx->width * enc_ctx->height > 1920 * 1080) {
        logger::error(TAG, "Video resolution too large, scaling down");
        enc_ctx->width = 1920;
        enc_ctx->height = 1080;
    }

    if (out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if ((ret = avcodec_open2(enc_ctx, encoder, NULL)) < 0) {
        logger::error(TAG, "Cannot open encoder: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    // 6. 创建输出流
    out_stream = avformat_new_stream(out_fmt_ctx, NULL);
    if (!out_stream) {
        logger::error(TAG, "Failed to create output stream");
        ret = AVERROR(ENOMEM);
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    if ((ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx)) < 0) {
        logger::error(TAG, "Failed to copy encoder parameters: %s", av_err2str(ret));
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }
    out_stream->time_base = enc_ctx->time_base;

    // 7. 初始化滤镜系统
    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        logger::error(TAG, "Unable to create filter graph");
        ret = AVERROR(ENOMEM);
        if (frame) av_frame_free(&frame);
        if (pkt) av_packet_free(&pkt);
        if (filter_graph) avfilter_graph_free(&filter_graph);
        if (enc_ctx) avcodec_free_context(&enc_ctx);
        if (dec_ctx) avcodec_free_context(&dec_ctx);
        if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&out_fmt_ctx->pb);
        if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
        if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
        return -1;
    }

    char args[512];
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             in_stream->time_base.num, in_stream->time_base.den,
             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    // 8. 创建滤镜链
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    const AVFilter *subtitles = avfilter_get_by_name("subtitles");

    // 创建输入buffer
    if ((ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in", args, NULL, filter_graph)) < 0) {
        logger::error(TAG, "Cannot create buffer source: %s", av_err2str(ret));
        goto end;
    }

    // 创建输出buffer
    if ((ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out", NULL, NULL, filter_graph)) < 0) {
        logger::error(TAG, "Cannot create buffer sink: %s", av_err2str(ret));
        goto end;
    }

    // 在创建滤镜前添加内存检查
    // av_mem_used
    // size_t free_mem = av_max_alloc() - av_mem_used();
    // if (free_mem < 50 * 1024 * 1024) { // 少于50MB时警告
    //     LOGW(TAG, "Low memory warning: %zuMB free", free_mem / (1024 * 1024));
    //     return AVERROR(ENOMEM);
    // }

    // 5. 安全创建字幕滤镜（关键修复）
    char filter_args[512];
    snprintf(filter_args, sizeof(filter_args),
             "filename='%s':"
             "fontsdir=/system/fonts/Roboto-Regular.ttf:"  // 指定单个字体
             "force_style="
             "FontSize=24,"
             "Outline=1,"
             "Shadow=0,"
             "MarginV=10",
             subtitlePath);

    // 创建字幕滤镜
    AVFilterContext *subtitles_ctx;
    snprintf(args, sizeof(args), "filename='%s':fontsdir=/system/fonts:force_style=Fontsize=24",
             subtitlePath);

    if ((ret = avfilter_graph_create_filter(&subtitles_ctx, subtitles, "subtitles", args, NULL, filter_graph)) < 0) {
        logger::error(TAG, "Cannot create subtitles filter: %s", av_err2str(ret));
        goto end;
    }

    // 6. 内存敏感操作区（添加保护）
    // AVFrame *frame = av_frame_alloc();
    // AVPacket *pkt = av_packet_alloc();
    // if (!frame || !pkt) {
    //     logger::error(TAG, "Frame/packet alloc failed");
    //     ret = AVERROR(ENOMEM);
    //     goto cleanup;
    // }



    // 连接滤镜
    if ((ret = avfilter_link(buffersrc_ctx, 0, subtitles_ctx, 0)) < 0) {
        logger::error(TAG, "Cannot link source to subtitles: %s", av_err2str(ret));
        goto end;
    }

    if ((ret = avfilter_link(subtitles_ctx, 0, buffersink_ctx, 0)) < 0) {
        logger::error(TAG, "Cannot link subtitles to sink: %s", av_err2str(ret));
        goto end;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        logger::error(TAG, "Cannot configure filter graph: %s", av_err2str(ret));
        goto end;
    }

    // 9. 打开输出文件
    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&out_fmt_ctx->pb, outputPath, AVIO_FLAG_WRITE)) < 0) {
            logger::error(TAG, "Could not open output file: %s", av_err2str(ret));
            goto end;
        }
    }

    // 10. 写入文件头
    if ((ret = avformat_write_header(out_fmt_ctx, NULL)) < 0) {
        logger::error(TAG, "Error writing header: %s", av_err2str(ret));
        goto end;
    }

    // 11. 初始化数据包和帧
    pkt = av_packet_alloc();
    frame = av_frame_alloc();
    if (!pkt || !frame) {
        logger::error(TAG, "Could not allocate packet/frame");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    // 12. 处理视频帧
    while (av_read_frame(in_fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            // 解码视频帧
            if ((ret = avcodec_send_packet(dec_ctx, pkt)) < 0) {
                logger::error(TAG, "Error sending packet to decoder: %s", av_err2str(ret));
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    logger::error(TAG, "Error during decoding: %s", av_err2str(ret));
                    goto end;
                }

                // 应用字幕滤镜
                if ((ret = av_buffersrc_add_frame(buffersrc_ctx, frame)) < 0) {
                    logger::error(TAG, "Error feeding filtergraph: %s", av_err2str(ret));
                    av_frame_unref(frame);
                    break;
                }

                // 获取处理后的帧
                while (true) {
                    ret = av_buffersink_get_frame(buffersink_ctx, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    else if (ret < 0) {
                        logger::error(TAG, "Error getting filtered frame: %s", av_err2str(ret));
                        break;
                    }

                    // 编码处理后的帧
                    if ((ret = avcodec_send_frame(enc_ctx, frame)) < 0) {
                        logger::error(TAG, "Error sending frame to encoder: %s", av_err2str(ret));
                        av_frame_unref(frame);
                        break;
                    }

                    while (true) {
                        ret = avcodec_receive_packet(enc_ctx, pkt);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                            break;
                        else if (ret < 0) {
                            logger::error(TAG, "Error during encoding: %s", av_err2str(ret));
                            break;
                        }

                        // 写入输出文件
                        pkt->stream_index = out_stream->index;
                        av_packet_rescale_ts(pkt, enc_ctx->time_base, out_stream->time_base);

                        if ((ret = av_interleaved_write_frame(out_fmt_ctx, pkt)) < 0) {
                            logger::error(TAG, "Error muxing packet: %s", av_err2str(ret));
                        }
                        av_packet_unref(pkt);
                    }
                    av_frame_unref(frame);
                }
            }
        }
        av_packet_unref(pkt);
    }

    // 13. 刷新编码器
    avcodec_send_frame(enc_ctx, NULL);
    while (true) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            break;
        else if (ret < 0) {
            logger::error(TAG, "Error flushing encoder: %s", av_err2str(ret));
            break;
        }

        pkt->stream_index = out_stream->index;
        av_packet_rescale_ts(pkt, enc_ctx->time_base, out_stream->time_base);

        if ((ret = av_interleaved_write_frame(out_fmt_ctx, pkt)) < 0) {
            logger::error(TAG, "Error muxing packet: %s", av_err2str(ret));
        }
        av_packet_unref(pkt);
    }

    // 14. 写入文件尾
    av_write_trailer(out_fmt_ctx);

    end:
    // 15. 释放资源
    if (frame) av_frame_free(&frame);
    if (pkt) av_packet_free(&pkt);
    if (filter_graph) avfilter_graph_free(&filter_graph);
    if (enc_ctx) avcodec_free_context(&enc_ctx);
    if (dec_ctx) avcodec_free_context(&dec_ctx);
    if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&out_fmt_ctx->pb);
    if (out_fmt_ctx) avformat_free_context(out_fmt_ctx);
    if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);

    return ret;
}

// int embedSubtitles(const char* inputPath, const char* subtitlePath, const char* outputPath) {
//     AVFormatContext *in_fmt_ctx = NULL, *out_fmt_ctx = NULL;
//     int ret;
//
//     // 1. 打开输入文件
//     if ((ret = avformat_open_input(&in_fmt_ctx, inputPath, NULL, NULL)) < 0) {
//         // 错误处理...
//     }
//
//     // 2. 创建输出上下文
//     avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, outputPath);
//
//     // 3. 复制视频和音频流
//     for (int i = 0; i < in_fmt_ctx->nb_streams; i++) {
//         AVStream *in_stream = in_fmt_ctx->streams[i];
//         AVStream *out_stream = avformat_new_stream(out_fmt_ctx, NULL);
//         avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
//     }
//
//     // 4. 添加字幕流
//     AVStream *subtitle_stream = avformat_new_stream(out_fmt_ctx, NULL);
//     AVCodecParameters *codecpar = subtitle_stream->codecpar;
//     codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
//     codecpar->codec_id = AV_CODEC_ID_SRT; // 或根据输入格式调整
//
//     // 5. 打开输出文件并写入头
//     avio_open(&out_fmt_ctx->pb, outputPath, AVIO_FLAG_WRITE);
//     avformat_write_header(out_fmt_ctx, NULL);
//
//     // 6. 读取并写入字幕数据
//     FILE *sub_file = fopen(subtitlePath, "r");
//     char line[1024];
//     int64_t pts = 0;
//
//     while (fgets(line, sizeof(line), sub_file)) {
//         AVPacket pkt = {0};
//         av_new_packet(&pkt, strlen(line));
//         memcpy(pkt.data, line, strlen(line));
//         pkt.stream_index = subtitle_stream->index;
//         pkt.pts = pts;
//         pkt.duration = 1000000; // 1秒
//
//         av_interleaved_write_frame(out_fmt_ctx, &pkt);
//         pts += 1000000;
//         av_packet_unref(&pkt);
//     }
//
//     // 7. 写入文件尾并清理资源...
// }