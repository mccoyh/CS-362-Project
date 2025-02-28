#ifndef AudioToTxt_H
#define AudioTotxt_H

#include <string>

namespace Captions{
int transcribeAudio(const std::string model_path, const std::string audio_file, const std::string output_srt);
}
#endif