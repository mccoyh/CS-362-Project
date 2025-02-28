#ifndef AudioToTxt_H
#define AudioTotxt_H

#include <string>
#include <filesystem>

bool extractAudio(const std::filesystem::path& mp4File, const std::filesystem::path& audioFile);

#endif