#ifndef AudioToTxt_H
#define AudioToTxt_H

#include <string>

namespace Captions{
int transcribeAudio(const std::string model_path, const std::string audio_file, const std::string output_srt);

std::string getSubtitleForFrame(int frame_number, const std::string& filename);
}
#endif