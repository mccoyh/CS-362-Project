#include "audio.h"
#include <iostream>
#include <stdexcept>
#include <filesystem>
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

    AudioData playAudio(const char* input_wav) {   
        //initizalize SDL
        SDL_Init(SDL_INIT_AUDIO);

        //SDL_AudioSpec spec;

        uint8_t *audio; //audio data
        uint32_t len; //audio data length, not duration of audio file
        AudioData ad;

        //load wav file
        if (SDL_LoadWAV(input_wav, &ad.spec, &audio, &len) == NULL){
            std::cout << "Error loading file" << std::endl;
            return ad;
        }

        ad.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &ad.spec, NULL, NULL);
        //ad.spec = spec;
        SDL_PutAudioStreamData(ad.stream, audio, sizeof(uint8_t)*len);
        SDL_ResumeAudioStreamDevice(ad.stream); // Start playback
        
        //Duration of wav file in milliseconds
        ad.duration = ((((len/(SDL_AUDIO_BITSIZE(ad.spec.format)/8)) / ad.spec.channels) / ad.spec.freq) * 1000) + 150; // extra 150 ms for buffer

        // while(duration > 0){
        //     SDL_Delay(10); // check for more inputs every 10 ms

        //     // add any input checks

        //     duration -= 10;
        // }

        // Cleanup
        // SDL_ClearAudioStream(stream);
        // SDL_DestroyAudioStream(stream);
        SDL_free(audio);
        // SDL_QuitSubSystem(SDL_INIT_AUDIO);
        // SDL_QuitSubSystem(SDL_INIT_EVENTS);
        // SDL_Quit();

        return ad;
    }

    void initSDL(){
        SDL_Init(SDL_INIT_AUDIO);
    }
    
    void delay(uint32_t ms){
        SDL_Delay(ms);
    }
    
    void deleteStream(SDL_AudioStream *stream){
        SDL_ClearAudioStream(stream);
        SDL_DestroyAudioStream(stream);
    }
    
    void quitSDL(){
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
        SDL_Quit();
    }

    void pauseAudio(SDL_AudioStream* stream){
        if (SDL_AudioStreamDevicePaused(stream)){
            SDL_PauseAudioStreamDevice(stream);
        } else {
            SDL_ResumeAudioStreamDevice(stream);
        }
    }

    bool isPaused(SDL_AudioStream* stream){
        return SDL_AudioStreamDevicePaused(stream);
    }
} // namespace Audio
