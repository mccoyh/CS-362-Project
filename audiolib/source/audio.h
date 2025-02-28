#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL_audio.h>
#include <string>

namespace Audio {

    struct AudioData {
        SDL_AudioStream* stream;    // audio stream
        SDL_AudioSpec spec;         // audio specifications
        uint32_t duration;          // audio duration (will be done in ms w/ 150 ms buffer)
    };

    // Checks if file exists
    bool exists(const std::string& name);

    // Converts any video or audio file to wav
    void convertWav(const std::string& input);

    // Plays audio file, must be wav format, convert if necessary
    // returns audiodata struct containing audio stream, audiospec, and duration
    AudioData playAudio(const char* input_wav);

    void initSDL();

    void delay(uint32_t ms);

    void deleteStream(SDL_AudioStream *stream);

    void quitSDL();

    void pauseAudio(SDL_AudioStream* stream);

    void resumeAudio(SDL_AudioStream* stream);

    bool isPaused(SDL_AudioStream* stream);
}

#endif