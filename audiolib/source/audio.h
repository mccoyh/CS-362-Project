#ifndef AUDIO_H
#define AUDIO_H

#include <string>

//checks if file exists
bool exists (const std::string& name);

//converts mp4 to mp3, pulls audio from video
void convert_mp4_mp3(std::string input_mp4);

//play audio file
void play_audio(std::string input_mp3);

#endif