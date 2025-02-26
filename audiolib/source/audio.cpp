#include "audio.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// Checks if file exists
bool exists (const std::string& name) {
    return std::filesystem::exists(name);
}

// Converts mp4 to wav, pulls audio from video
void convertWav(const std::string& input) {
    if(exists(input))
    {
        //checks for any existing output.wav files that already exist
        int num = 0;
        while(exists("output" + std::to_string(num) + ".wav"))
        {
            num++;
        }
        system(("ffmpeg -i " + input + " output" + std::to_string(num) + ".wav").c_str());
    }
}

bool play_audio(const char* input_wav) {   
    //initizalize SDL
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec;

    uint8_t *wavStart;
    uint32_t wavLen;

    //load wav file
    if (SDL_LoadWAV(input_wav, &spec, &wavStart, &wavLen) == NULL){
        std::cout << "Error loading file" << std::endl;
        return false;
    } else {
        std::cout << "File loaded" << std::endl;
    }

    uint8_t *audio = wavStart; //audio data
    uint32_t len = wavLen; //audio data length, not duration of audio file

    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_PutAudioStreamData(stream, audio, sizeof(uint8_t)*len);
    SDL_ResumeAudioStreamDevice(stream); // Start playback

    uint32_t duration = (((len/(SDL_AUDIO_BITSIZE(spec.format)/8)) / spec.channels) / spec.freq) * 1000; //Duration of wav file in milliseconds

    SDL_Delay(duration+150); // extra 150 ms for buffer

    // Cleanup
    SDL_ClearAudioStream(stream);
    SDL_DestroyAudioStream(stream);
    SDL_free(wavStart);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();

    return true;
}