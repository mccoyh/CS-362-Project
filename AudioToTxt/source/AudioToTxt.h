#ifndef AudioToTxt_H
#define AudioTotxt_H



#include <string>

void extractAudio(const std::string& mp4File, const std::string& audioFile);

int extract_audio(const char* input_file, const char* output_audio);

// int transcribe_audio(const char* model_path, const char* audio_file, const char* output_srt);

// int add_subtitles(const char* input_video, const char* subtitle_file, const char* output_video) 

#endif