#include <iostream>
#include <string>
#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

void extractFrames(const std::string& inputFile) {
    AVFormatContext* formatContext = nullptr;
    if (avformat_open_input(&formatContext, inputFile.c_str(), nullptr, nullptr) != 0) {
        throw std::runtime_error("Failed to open input file");
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        throw std::runtime_error("Failed to retrieve stream info");
    }

    // Print metadata
    std::cout << "Video Metadata:" << std::endl;
    std::cout << "Format: " << formatContext->iformat->name << std::endl;
    std::cout << "Duration: " << formatContext->duration / AV_TIME_BASE << " seconds" << std::endl;

    // Find the video stream and display its information
    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        throw std::runtime_error("No video stream found");
    }

    AVCodecParameters* codecParameters = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        throw std::runtime_error("Codec not found");
    }

    // Print codec details
    std::cout << "Codec: " << codec->name << std::endl;
    std::cout << "Resolution: " << codecParameters->width << "x" << codecParameters->height << std::endl;
    std::cout << "Bitrate: " << codecParameters->bit_rate / 1000 << " kbps" << std::endl;

    // Print frame rate
    AVRational frameRate = formatContext->streams[videoStreamIndex]->avg_frame_rate;
    std::cout << "Frame rate: " << frameRate.num << "/" << frameRate.den << " fps" << std::endl;

    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        throw std::runtime_error("Failed to allocate codec context");
    }

    if (avcodec_parameters_to_context(codecContext, codecParameters) < 0) {
        throw std::runtime_error("Failed to copy codec parameters to context");
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        throw std::runtime_error("Failed to open codec");
    }

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();

    int width = codecContext->width;
    int height = codecContext->height;
    SwsContext* swsContext = sws_getContext(width, height, codecContext->pix_fmt, width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    int rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    uint8_t* rgbBuffer = (uint8_t*)av_malloc(rgbBufferSize * sizeof(uint8_t));
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, rgbBuffer, AV_PIX_FMT_RGB24, width, height, 1);

    int frameCount = 0;
    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, height, rgbFrame->data, rgbFrame->linesize);

                    // Save frame as .ppm image (optional)
                    std::string filename = "frame_" + std::to_string(frameCount++) + ".ppm";
                    FILE* file = fopen(filename.c_str(), "wb");
                    fprintf(file, "P6\n%d %d\n255\n", width, height);
                    fwrite(rgbFrame->data[0], 1, rgbBufferSize, file);
                    fclose(file);
                }
            }
        }
        av_packet_unref(packet);
    }

    av_free(rgbBuffer);
    av_frame_free(&rgbFrame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    sws_freeContext(swsContext);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

int main() {
    try {
        extractFrames("assets/turtles.mp4");  // Replace with your input file path
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }

    return 0;
}
