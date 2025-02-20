#include <source/AudioToTxt.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

int main() {
    const char* audio_file = "audio.pcm";
    const char* subtitle_file = "subtitles.srt";
    const char* output_video = "output_with_subtitles.mp4";
    

    std::filesystem::path exe_path = std::filesystem::current_path(); // Gets the current working directory (where executable runs)
    
    std::string model_path = exe_path.string();

    // Path to the assets folder relative to the executable's directory
    std::string assets_path = exe_path.parent_path().string() + "/AudioToTxt/assets/";

    

    std::string input_video = assets_path + "CS_test.mp4";

    std::cout << "Assests path: " << assets_path << std::endl << "Model path: "<< model_path << std::endl <<  "Input file: " << input_video << std::endl;
    //if (extract_audio(input_video, audio_file) != 0) return -1;
    //if (transcribe_audio(model_path, audio_file, subtitle_file) != 0) return -1;
    //if (add_subtitles(input_video, subtitle_file, output_video) != 0) return -1;

    return 0;
}


