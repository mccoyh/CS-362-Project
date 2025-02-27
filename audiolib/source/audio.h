#ifndef AUDIO_H
#define AUDIO_H

#include <string>

namespace Audio {

    // Checks if file exists
    bool exists(const std::string& name);

    // Converts any video or audio file to wav
    void convertWav(const std::string& input);

    // Plays audio file, must be wav format, convert if necessary
    bool playAudio(const char* input_wav);

}

#endif