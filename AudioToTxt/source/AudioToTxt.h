#ifndef AudioToTxt_H
#define AudioTotxt_H



#include <string>
#include <filesystem>

bool extractAudio(const std::filesystem::path& mp4File, const std::filesystem::path& audioFile);


int transcribe_audio(const std::filesystem::path model_path, const std::filesystem::path audio_file, const std::filesystem::path output_srt);

// int add_subtitles(const char* input_video, const char* subtitle_file, const char* output_video) 

#endif