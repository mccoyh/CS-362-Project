#include "AudioToTxt.h"

// Function to extract audio from an MP4 file using FFmpeg libraries
void extractAudio(const std::string& mp4File, const std::string& audioFile) {
    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, mp4File.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Error: Could not open input file " << mp4File << std::endl;
        exit(1);
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Error: Could not find stream info" << std::endl;
        exit(1);
    }

    int audioStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    if (audioStreamIndex == -1) {
        std::cerr << "Error: No audio stream found" << std::endl;
        exit(1);
    }

    AVCodecParameters* codecParams = formatContext->streams[audioStreamIndex]->codecpar;
    AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        std::cerr << "Error: Codec not found" << std::endl;
        exit(1);
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
        std::cerr << "Error: Could not initialize codec context" << std::endl;
        exit(1);
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "Error: Could not open codec" << std::endl;
        exit(1);
    }

    AVPacket packet;
    av_init_packet(&packet);
    AVFrame* frame = av_frame_alloc();
    FILE* outFile = fopen(audioFile.c_str(), "wb");

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == audioStreamIndex) {
            if (avcodec_receive_frame(codecContext, frame) == 0) {
                // Write audio to file (assuming PCM for simplicity)
                fwrite(frame->data[0], 1, frame->linesize[0], outFile);
            }
        }
        av_packet_unref(&packet);
    }

    fclose(outFile);
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}