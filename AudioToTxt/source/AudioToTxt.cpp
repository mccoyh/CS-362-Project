#include "AudioToTxt.h"
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

#include "whisper.h"


// extern "C" {
//     #include <libavformat/avformat.h>
//     #include <libavcodec/avcodec.h>
//     #include <libavutil/avutil.h>
//     #include <libswresample/swresample.h>
//     #include <libavutil/opt.h>
// }
    



    
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
        exit(1);
    }

    //def output channel layout
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 1); //layout is AV_CH_LAYOUT_MONO


    av_opt_set_chlayout(swrContext, "in_chlayout", &codecContext->ch_layout, 0); 
    av_opt_set_int(swrContext, "in_sample_fmt", codecContext->sample_fmt, 0);
    av_opt_set_int(swrContext, "in_sample_rate", codecContext->sample_rate, 0);
    av_opt_set_chlayout(swrContext, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int(swrContext, "out_sample_rate", 16000, 0);



    // Initialize resampler
    if (swr_init(swrContext) < 0) {
        std::cout << "Error: Could not initialize SwrContext" << std::endl;
        swr_free(&swrContext);
        exit(1);
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
                                                        (const uint8_t**)frame->data, frame->nb_samples);
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
    

int transcribe_audio(const std::filesystem::path model_path, const std::filesystem::path audio_file, const std::filesystem::path output_srt) {
    std::cout << "in function  " << std::endl;
    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    whisper_context* ctx = whisper_init_from_file(model_path.string().c_str());
    if (!ctx) {
        std::cerr << "Failed to initialize Whisper model from file: " << model_path << std::endl;
        return -1;
    }

    std::vector<float> pcm_data; // Store extracted audio samples

    // Load the extracted audio file
    std::ifstream input(audio_file, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Failed to open audio file: " << audio_file << std::endl;
        whisper_free(ctx);
        return -1;
    }

    input.seekg(0, std::ios::end);
    size_t size = input.tellg();
    input.seekg(0, std::ios::beg);


    // Resize the vector based on the size of the input
    pcm_data.resize(size / sizeof(short));  // sizeof(short) because it's 16-bit PCM

    // Read PCM data
    std::vector<short> pcm_short_data(pcm_data.size());
    input.read(reinterpret_cast<char*>(pcm_short_data.data()), size);
    input.close();

    // Convert PCM data (16-bit signed) to floating point
    for (size_t i = 0; i < pcm_short_data.size(); ++i) {
        pcm_data[i] = pcm_short_data[i] / 32768.0f; // Convert to float in the range of -1.0 to 1.0
    }

    // Transcribe audio
    
    int result = whisper_full(ctx, params, pcm_data.data(), pcm_data.size());
    if (result != 0) {
        std::cerr << "Whisper transcription failed." << std::endl;
        whisper_free(ctx);
        return -1;
    }
    // Save subtitles
    std::ofstream srt_out(output_srt);
    if (!srt_out.is_open()) {
        std::cerr << "Failed to open subtitle file: " << output_srt << std::endl;
        whisper_free(ctx);
        return -1;
    }
    std::cout << "reaches write  " << std::endl;
    for (int i = 0; i < whisper_full_n_segments(ctx); i++) {
        std::cout << "segment: " << i << std::endl;
        srt_out << i + 1 << "\n";
        srt_out << whisper_full_get_segment_t0(ctx, i) << " --> "
                << whisper_full_get_segment_t1(ctx, i) << "\n";
        srt_out << whisper_full_get_segment_text(ctx, i) << "\n\n";
    }

    srt_out.close();
    whisper_free(ctx);
    return 0;
}

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



    
    // bool extractAudioFromVideo(const std::string &videoFilePath, const std::string &audioOutputPath) {
    //     AVFormatContext *fmt_ctx = nullptr;
    //     AVCodecContext *dec_ctx = nullptr;
    //     AVCodec *decoder = nullptr;
    //     AVStream *audio_stream = nullptr;
    //     int audio_stream_index = -1;
    //     AVPacket pkt;
    //     AVFrame *frame = nullptr;
    //     SwrContext *swr_ctx = nullptr;
    //     FILE *outputFile = nullptr;
    
    //     // Initialize FFmpeg
    //     av_register_all();
    //     avformat_network_init();
    
    //     // Open video file
    //     if (avformat_open_input(&fmt_ctx, videoFilePath.c_str(), nullptr, nullptr) < 0) {
    //         std::cerr << "Could not open input file: " << videoFilePath << std::endl;
    //         return false;
    //     }
    
    //     // Retrieve stream information
    //     if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
    //         std::cerr << "Could not find stream information" << std::endl;
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Find the first audio stream
    //     for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
    //         if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
    //             audio_stream_index = i;
    //             audio_stream = fmt_ctx->streams[i];
    //             break;
    //         }
    //     }
    
    //     if (audio_stream_index == -1) {
    //         std::cerr << "Could not find an audio stream in the file" << std::endl;
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Find the decoder for the audio stream
    //     decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    //     if (!decoder) {
    //         std::cerr << "Codec not found" << std::endl;
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Allocate codec context and open the decoder
    //     dec_ctx = avcodec_alloc_context3(decoder);
    //     if (!dec_ctx) {
    //         std::cerr << "Could not allocate codec context" << std::endl;
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     if (avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar) < 0) {
    //         std::cerr << "Could not copy codec parameters to codec context" << std::endl;
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     if (avcodec_open2(dec_ctx, decoder, nullptr) < 0) {
    //         std::cerr << "Could not open codec" << std::endl;
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Open output WAV file
    //     outputFile = fopen(audioOutputPath.c_str(), "wb");
    //     if (!outputFile) {
    //         std::cerr << "Could not open output file: " << audioOutputPath << std::endl;
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Set up swresample context to convert audio to 16kHz mono WAV
    //     swr_ctx = swr_alloc();
    //     if (!swr_ctx) {
    //         std::cerr << "Could not allocate swresample context" << std::endl;
    //         fclose(outputFile);
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     av_opt_set_int(swr_ctx, "in_channel_layout", dec_ctx->channel_layout, 0);
    //     av_opt_set_int(swr_ctx, "in_sample_rate", dec_ctx->sample_rate, 0);
    //     av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", dec_ctx->sample_fmt, 0);
    //     av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    //     av_opt_set_int(swr_ctx, "out_sample_rate", 16000, 0);
    //     av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    
    //     if (swr_init(swr_ctx) < 0) {
    //         std::cerr << "Could not initialize swresample context" << std::endl;
    //         fclose(outputFile);
    //         swr_free(&swr_ctx);
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     frame = av_frame_alloc();
    //     if (!frame) {
    //         std::cerr << "Could not allocate frame" << std::endl;
    //         fclose(outputFile);
    //         swr_free(&swr_ctx);
    //         avcodec_free_context(&dec_ctx);
    //         avformat_close_input(&fmt_ctx);
    //         return false;
    //     }
    
    //     // Process audio frames
    //     while (av_read_frame(fmt_ctx, &pkt) >= 0) {
    //         if (pkt.stream_index == audio_stream_index) {
    //             // Decode audio
    //             if (avcodec_send_packet(dec_ctx, &pkt) < 0) {
    //                 std::cerr << "Error sending packet to decoder" << std::endl;
    //                 break;
    //             }
    
    //             while (avcodec_receive_frame(dec_ctx, frame) >= 0) {
    //                 uint8_t *out_buffer = nullptr;
    //                 int out_buffer_size = 0;
    //                 swr_convert(swr_ctx, &out_buffer, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
    
    //                 // Write to output WAV file
    //                 fwrite(out_buffer, 1, out_buffer_size, outputFile);
    //             }
    //         }
    //         av_packet_unref(&pkt);
    //     }
    
    //     // Cleanup
    //     av_frame_free(&frame);
    //     swr_free(&swr_ctx);
    //     fclose(outputFile);
    //     avcodec_free_context(&dec_ctx);
    //     avformat_close_input(&fmt_ctx);
    
    //     std::cout << "Audio extracted successfully to " << audioOutputPath << std::endl;
    //     return true;
    // }

