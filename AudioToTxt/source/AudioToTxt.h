#ifndef AudioToTxt_H
#define AudioTotxt_H

#include <string>
#include <filesystem>

int transcribeAudio(const std::filesystem::path model_path, const std::filesystem::path audio_file, const std::filesystem::path output_srt);

#endif