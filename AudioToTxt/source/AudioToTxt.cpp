#include "AudioToTxt.h"
#include <iostream>
#include <fstream>
#include <vector>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include "whisper.h"


// extern "C" {
//     #include <libavformat/avformat.h>
//     #include <libavcodec/avcodec.h>
//     #include <libavutil/avutil.h>
//     #include <libswresample/swresample.h>
//     #include <libavutil/opt.h>
// }
    


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
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
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


int extract_audio(const char* input_file, const char* output_audio) {
    avformat_network_init();
    AVFormatContext* format_ctx = nullptr;
    
    // Open video file
    if (avformat_open_input(&format_ctx, input_file, nullptr, nullptr) != 0) {
        std::cerr << "Could not open video file!" << std::endl;
        return -1;
    }

    // Find audio stream
    int audio_stream_index = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
            break;
        }
    }

    if (audio_stream_index == -1) {
        std::cerr << "No audio stream found!" << std::endl;
        avformat_close_input(&format_ctx);
        return -1;
    }

    AVCodecParameters* codecpar = format_ctx->streams[audio_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, codec, nullptr);

    // Initialize resampler to output 16-bit PCM audio (for Whisper)
    SwrContext* swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", codec_ctx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr, "in_sample_rate", codec_ctx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", 16000, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", codec_ctx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    std::ofstream audio_out(output_audio, std::ios::binary);
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();

    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == audio_stream_index) {
            avcodec_send_packet(codec_ctx, &packet);
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                int buffer_size = av_samples_get_buffer_size(
                    nullptr, 1, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
                uint8_t* buffer = new uint8_t[buffer_size];
                swr_convert(swr, &buffer, frame->nb_samples, 
                            (const uint8_t**)frame->data, frame->nb_samples);
                audio_out.write(reinterpret_cast<char*>(buffer), buffer_size);
                delete[] buffer;
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    audio_out.close();
    av_frame_free(&frame);
    swr_free(&swr);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    return 0;
}



// int transcribe_audio(const char* model_path, const char* audio_file, const char* output_srt) {
//     whisper_context* ctx = whisper_init(model_path);
//     std::vector<float> pcm_data; // Store extracted audio samples

//     // Load the extracted audio file
//     std::ifstream input(audio_file, std::ios::binary);
//     input.seekg(0, std::ios::end);
//     size_t size = input.tellg();
//     input.seekg(0, std::ios::beg);
//     pcm_data.resize(size / sizeof(float));
//     input.read(reinterpret_cast<char*>(pcm_data.data()), size);
//     input.close();

//     // Transcribe audio
//     whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
//     whisper_full(ctx, params, pcm_data.data(), pcm_data.size());

//     // Save subtitles
//     std::ofstream srt_out(output_srt);
//     for (int i = 0; i < whisper_full_n_segments(ctx); i++) {
//         srt_out << i + 1 << "\n";
//         srt_out << whisper_full_get_segment_t0(ctx, i) << " --> "
//                 << whisper_full_get_segment_t1(ctx, i) << "\n";
//         srt_out << whisper_full_get_segment_text(ctx, i) << "\n\n";
//     }

//     srt_out.close();
//     whisper_free(ctx);
//     return 0;
// }

// int add_subtitles(const char* input_video, const char* subtitle_file, const char* output_video) {
//     AVFormatContext* format_ctx = nullptr;
    
//     // Open video file
//     if (avformat_open_input(&format_ctx, input_video, nullptr, nullptr) != 0) {
//         std::cerr << "Could not open video file!" << std::endl;
//         return -1;
//     }

//     // Initialize FFmpeg filter to apply subtitles
//     AVFilterGraph* filter_graph = avfilter_graph_alloc();
//     AVFilterContext* buffer_src_ctx = nullptr;
//     AVFilterContext* buffer_sink_ctx = nullptr;

//     // Create filter to overlay subtitles
//     const char* filter_desc = "subtitles=subtitles.srt";
//     AVFilterInOut* inputs = avfilter_inout_alloc();
//     AVFilterInOut* outputs = avfilter_inout_alloc();
    
//     avfilter_graph_parse_ptr(filter_graph, filter_desc, &inputs, &outputs, nullptr);
//     avfilter_graph_config(filter_graph, nullptr);

//     // Process video and apply filter (pseudo-code for brevity)
//     // Iterate frames and encode video with subtitles applied
//     // ...

//     // Cleanup
//     avfilter_graph_free(&filter_graph);
//     avformat_close_input(&format_ctx);
//     return 0;
// }



    
    bool extractAudioFromVideo(const std::string &videoFilePath, const std::string &audioOutputPath) {
        AVFormatContext *fmt_ctx = nullptr;
        AVCodecContext *dec_ctx = nullptr;
        AVCodec *decoder = nullptr;
        AVStream *audio_stream = nullptr;
        int audio_stream_index = -1;
        AVPacket pkt;
        AVFrame *frame = nullptr;
        SwrContext *swr_ctx = nullptr;
        FILE *outputFile = nullptr;
    
        // Initialize FFmpeg
        av_register_all();
        avformat_network_init();
    
        // Open video file
        if (avformat_open_input(&fmt_ctx, videoFilePath.c_str(), nullptr, nullptr) < 0) {
            std::cerr << "Could not open input file: " << videoFilePath << std::endl;
            return false;
        }
    
        // Retrieve stream information
        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            std::cerr << "Could not find stream information" << std::endl;
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Find the first audio stream
        for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_stream_index = i;
                audio_stream = fmt_ctx->streams[i];
                break;
            }
        }
    
        if (audio_stream_index == -1) {
            std::cerr << "Could not find an audio stream in the file" << std::endl;
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Find the decoder for the audio stream
        decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
        if (!decoder) {
            std::cerr << "Codec not found" << std::endl;
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Allocate codec context and open the decoder
        dec_ctx = avcodec_alloc_context3(decoder);
        if (!dec_ctx) {
            std::cerr << "Could not allocate codec context" << std::endl;
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        if (avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar) < 0) {
            std::cerr << "Could not copy codec parameters to codec context" << std::endl;
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        if (avcodec_open2(dec_ctx, decoder, nullptr) < 0) {
            std::cerr << "Could not open codec" << std::endl;
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Open output WAV file
        outputFile = fopen(audioOutputPath.c_str(), "wb");
        if (!outputFile) {
            std::cerr << "Could not open output file: " << audioOutputPath << std::endl;
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Set up swresample context to convert audio to 16kHz mono WAV
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            std::cerr << "Could not allocate swresample context" << std::endl;
            fclose(outputFile);
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        av_opt_set_int(swr_ctx, "in_channel_layout", dec_ctx->channel_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", dec_ctx->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", dec_ctx->sample_fmt, 0);
        av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", 16000, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    
        if (swr_init(swr_ctx) < 0) {
            std::cerr << "Could not initialize swresample context" << std::endl;
            fclose(outputFile);
            swr_free(&swr_ctx);
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        frame = av_frame_alloc();
        if (!frame) {
            std::cerr << "Could not allocate frame" << std::endl;
            fclose(outputFile);
            swr_free(&swr_ctx);
            avcodec_free_context(&dec_ctx);
            avformat_close_input(&fmt_ctx);
            return false;
        }
    
        // Process audio frames
        while (av_read_frame(fmt_ctx, &pkt) >= 0) {
            if (pkt.stream_index == audio_stream_index) {
                // Decode audio
                if (avcodec_send_packet(dec_ctx, &pkt) < 0) {
                    std::cerr << "Error sending packet to decoder" << std::endl;
                    break;
                }
    
                while (avcodec_receive_frame(dec_ctx, frame) >= 0) {
                    uint8_t *out_buffer = nullptr;
                    int out_buffer_size = 0;
                    swr_convert(swr_ctx, &out_buffer, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
    
                    // Write to output WAV file
                    fwrite(out_buffer, 1, out_buffer_size, outputFile);
                }
            }
            av_packet_unref(&pkt);
        }
    
        // Cleanup
        av_frame_free(&frame);
        swr_free(&swr_ctx);
        fclose(outputFile);
        avcodec_free_context(&dec_ctx);
        avformat_close_input(&fmt_ctx);
    
        std::cout << "Audio extracted successfully to " << audioOutputPath << std::endl;
        return true;
    }

