#ifndef AUDIO_H
#define AUDIO_H

#include <string>

// Checks if file exists
bool exists(const std::string& name);

// Converts mp4 to mp3, pulls audio from video
void convert_mp4_mp3(const std::string& input_mp4);

// Plays audio file
void play_audio(const std::string& input_mp3);

#endif