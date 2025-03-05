
# About
This c++ library uses FFmpeg, Whisper ASR cpp, and Vulkan SDK to create subtitles for videos. The library works with any whisper model and provides a human readable subtitle file with timestamps in frames. 

# Notes
When running whisper for the first time with vulkan, the vulkan shaders are compiled extending the runtime for the first use. 

If vulkan is not enabled, use smaller models such as base, tiny, or small.

# Build Options
- WHISPER_USE_VULKAN - Default ON: This flag can be used to turn on or off building the library with vulkan support. Vulkan provides GPU acceleration lowering the runtime of models. It is recommended to have Vulkan on for any large models.  


# Functions
## transcribeAudio(const std::string& model_path, const std::string& audio_file, const std::string& output_srt)

This function uses whisper ASR cpp to transcribe the audio into subtitles and store the subtitles in a .srt file. 

Subtitles are formatted:  
index_number\n
start_frame --> end_frame\n 
subtitle_text\n
\n  


#### Parameters
- model_file :   
    The location of a whisper model. 
- audio_file :  
    The location of a formatted audio file (see whisper documentation for format).
- output_srt :  
    The location to save the generated subtitles. 
#### Returns
- Generated subtitles are saved to the output_srt file location. 
- int : Returns 1 if success full and -1 otherwise. 


## getSubtitleForFrame(int frame_number, const std::string& filename)

This function searches the subtitle file for subtitles at a specific frame. The file must be formatted as generated in the transcribeAudio function. 
#### Parameters
 - frame : 
    The frame to find subtitles at.
 - subtitle_file : 
    The location of the subtitle text file.

#### Returns
 - std::string : The subtitle text


# Classes
## CaptionCache
### Constructor
#### CaptionCache(const std::string& caption_srt)
- caption_srt : The path to a caption file.

### Methods
#### loadCaptions(const std::string& caption_srt)
This function loads a subtitle file into a map for quicker searching and accessing.
##### Parameters
- caption_srt : The path to a caption file.
#### getCaptionAtFrame(int frame)
This function returns the subtitle for a given frame.
##### Parameters
- frame : The frame to retrieve a subtitle.
#### getNumCaptions()
This function gets the number of subtitle blocks stored in the class.
##### Returns
- int : The number of subtitle blocks.



# Example Usage
```cpp
#include <source/AudioToTxt.h>
#include <filesystem>
#include <iostream>

#include "audioDecoding.h"

int main(const int argc, char* argv[]) {
    std::filesystem::path exePath = std::filesystem::current_path(); // Gets the current working directory (where executable runs)

    // Path to the assets folder relative to the executable's directory
    std::filesystem::path assetsPath =  exePath / "assets";
    std::filesystem::path inputVideo = (argc == 2 ? argv[1] : "assets/CS_test.mp4");

    const std::filesystem::path audioFile = assetsPath / "audio.pcm";
    const std::filesystem::path subtitleFile = assetsPath / "subtitles.srt";
    const std::filesystem::path outputVideo = assetsPath / "output_with_subtitles_turbo.mp4";
    const std::filesystem::path modelPath = exePath / "models" / "ggml-large-v3-turbo-q5_0.bin"; //"ggml-base.bin"
    // Format Audio for Whisper
    if (!extractAudio(inputVideo.string(), audioFile.string())){
        std::cout << "Failed to generate formated audio file" << std::endl;
    }
    //Transcribe Audio
    if (Captions::transcribeAudio(modelPath.string(), audioFile.string(), subtitleFile.string()) != 0){
        std::cout << "Failed to generate subtitles" << std::endl;
    }
    //Get caption at frame from subtitle file
    int frame = 200; 
    std::string caption = Captions::getSubtitleForFrame(frame, subtitleFile.string());
    if (caption.at(0) == '['){
        std::cout << "Failed to find subtitle at frame. message: "<< caption << std::endl;
    }
    else{
        std::cout << "Caption at frame " << frame << ": " << caption << std::endl;
    }
    //Cache captions and get subtitle at frame
    Captions::CaptionCache cache(subtitleFile.string());

    std::string captionFromCache = cache.getCaptionAtFrame(frame);
    std::cout << "Cached caption at frame " << frame << ": " << captionFromCache << std::endl;
    std::cout << "Number of captions: " << cache.getNumCaptions() << std::endl;

    return 0;
}
```