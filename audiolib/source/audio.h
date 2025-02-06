#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <sys/stat.h>
//#include <unistd.h>
#include <fstream>
#include <cstdlib>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
//#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}
#include <SDL2/SDL.h>

//checks if file exists
inline bool exists (const std::string& name);

//converts mp4 to mp3, pulls audio from video
void convert_mp4_mp3(std::string input_mp4);

//play audio file
void play_audio(std::string input_mp3);

#endif