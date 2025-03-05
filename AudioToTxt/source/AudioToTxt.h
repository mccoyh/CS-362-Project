#ifndef AudioToTxt_H
#define AudioToTxt_H

#include <string>
#include <map>

namespace Captions{
int transcribeAudio(const std::string& model_path, const std::string& audio_file, const std::string& output_srt);

std::string getSubtitleForFrame(int frame_number, const std::string& filename);

struct Subtitle {
    int startFrame = -1;  // Start frame
    int endFrame = -1;    // End frame
    std::string text = "";  // Subtitle text

    Subtitle()
         : startFrame(-1), endFrame(-1), text(""){}

    Subtitle(int start, int end, const std::string& txt)
         : startFrame(start), endFrame(end), text(txt){}
};

class CaptionCache{
    public:
    explicit CaptionCache(const std::string& caption_srt);
    void loadCaptions(const std::string& caption_srt);
    std::string getCaptionAtFrame(int frame);
    int getNumCaptions();
    
    private:
        std::map<double, Subtitle> captionMap;
        int numCaptions;
};
}
#endif