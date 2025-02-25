#include "AudioToTxt.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "whisper.h"
    
int transcribeAudio(const std::filesystem::path model_path, const std::filesystem::path audio_file, const std::filesystem::path output_srt) {
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