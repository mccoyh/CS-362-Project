#include "audio.h"
#include <stdexcept>
#include <SDL3/SDL_audio.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// Checks if file exists
bool exists (const std::string& name)
{
    struct stat buffer;
    return stat (name.c_str(), &buffer) == 0;
}

// Converts mp4 to mp3, pulls audio from video
void convert_mp4_mp3(const std::string& input_mp4)
{
    if(exists(input_mp4))
    {
        //checks for any existing output.mp3 files that already exist
        int num = 0;
        while(exists("output" + std::to_string(num) + ".mp3"))
        {
            num++;
        }
        system(("ffmpeg -i " + input_mp4 + " output" + std::to_string(num) + ".mp3").c_str());
    }
}

void play_audio(const std::string& input_mp3)
{
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, input_mp3.c_str(), nullptr, nullptr) != 0)
    {
        throw std::runtime_error("failed to open input file");
    }

    int audioStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; i++)
    {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStreamIndex = static_cast<int>(i);
            break;
        }
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar);
    const AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
    avcodec_open2(codecContext, codec, nullptr);

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = codecContext->sample_rate;
    // desiredSpec.format = AUDIO_S16SYS;
    // desiredSpec.channels = codecContext->channels;
    // desiredSpec.samples = 1024;
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(0, &desiredSpec);
    SDL_PauseAudioDevice(0); // Start playback
}
