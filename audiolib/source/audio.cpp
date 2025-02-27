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

namespace Audio {

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

    bool playAudio(const char* input_wav) {   
        //initizalize SDL
        SDL_Init(SDL_INIT_AUDIO);

        SDL_AudioSpec spec;

        uint8_t *audio; //audio data
        uint32_t len; //audio data length, not duration of audio file

        //load wav file
        if (SDL_LoadWAV(input_wav, &spec, &audio, &len) == NULL){
            std::cout << "Error loading file" << std::endl;
            return false;
        }

        SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
        SDL_PutAudioStreamData(stream, audio, sizeof(uint8_t)*len);
        SDL_ResumeAudioStreamDevice(stream); // Start playback
        
        //Duration of wav file in milliseconds
        uint32_t duration = ((((len/(SDL_AUDIO_BITSIZE(spec.format)/8)) / spec.channels) / spec.freq) * 1000) + 150; // extra 150 ms for buffer

        while(duration > 0){
            SDL_Delay(10); // check for more inputs every 10 ms

            // add any input checks

            duration -= 10;
        }

        // Cleanup
        SDL_ClearAudioStream(stream);
        SDL_DestroyAudioStream(stream);
        SDL_free(audio);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
        SDL_Quit();

        return true;
    }
} // namespace Audio
