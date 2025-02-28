#include "audioDecoding.h"
#include <source/AudioToTxt.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/channel_layout.h>
}
    
bool extractAudio(const std::filesystem::path& mp4File, const std::filesystem::path& audioFile) {
    //Open video file
    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, mp4File.string().c_str(), nullptr, nullptr) != 0) {
        std::cout << "Error: Could not open input file " << mp4File << std::endl;
        return false;
    }
    //get streams
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cout << "Error: Could not find stream info" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }
    //find audio stream
    int audioStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    if (audioStreamIndex == -1) {
        std::cout << "Error: No audio stream found" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }
    //Determine codec and decoder
    AVCodecParameters* codecParams = formatContext->streams[audioStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        std::cout << "Error: Codec not found" << std::endl;
        avformat_close_input(&formatContext);
        return false;
    }
    //init codec
    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
        std::cout << "Error: Could not initialize codec context" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }
    //open codec
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cout << "Error: Could not open codec" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }
    //allocate swr context
    SwrContext* swrContext = swr_alloc();
    if (!swrContext) {
        std::cout << "Error: Could not allocate SwrContext" << std::endl;
        return false;
    }

    //def output channel layout
    AVChannelLayout outChLayout;
    av_channel_layout_default(&outChLayout, 1); //layout is AV_CH_LAYOUT_MONO

    av_opt_set_chlayout(swrContext, "in_chlayout", &codecContext->ch_layout, 0); 
    av_opt_set_int(swrContext, "in_sample_fmt", codecContext->sample_fmt, 0);
    av_opt_set_int(swrContext, "in_sample_rate", codecContext->sample_rate, 0);
    av_opt_set_chlayout(swrContext, "out_chlayout", &outChLayout, 0);
    av_opt_set_int(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swrContext, "out_sample_rate", 16000, 0);

    // Initialize resampler
    if (swr_init(swrContext) < 0) {
        std::cout << "Error: Could not initialize SwrContext" << std::endl;
        swr_free(&swrContext);
        return false;
    }

    //open output
    std::ofstream outFile(audioFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cout << "Error: Could not open output audio file." << std::endl;
        swr_free(&swrContext);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return false;
    }
    //Convert Audio by packet
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    
    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == audioStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    uint8_t* out_data = nullptr;
                    int out_samples = av_rescale_rnd(swr_get_delay(swrContext, codecContext->sample_rate) + frame->nb_samples,
                                                        16000, codecContext->sample_rate, AV_ROUND_UP);
                    av_samples_alloc(&out_data, nullptr, 1, out_samples, AV_SAMPLE_FMT_S16, 0);
                    int samples_converted = swr_convert(swrContext, &out_data, out_samples,
                                                        static_cast<uint8_t**>(frame->data), frame->nb_samples);
                    if (samples_converted > 0) {
                        outFile.write(reinterpret_cast<char*>(out_data), samples_converted * 2);
                    }
                    av_freep(&out_data);
                }
            }
        }
        av_packet_unref(&packet);
    }
    // Cleanup
    outFile.close();
    swr_free(&swrContext);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    return true;
}