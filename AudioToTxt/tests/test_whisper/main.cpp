#include <source/AudioToTxt.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

int main() {
   
    

    std::filesystem::path exe_path = std::filesystem::current_path(); // Gets the current working directory (where executable runs)

    // Path to the assets folder relative to the executable's directory
    std::filesystem::path assets_path =  exe_path / "assets";
    std::filesystem::path input_video = assets_path / "CS_test.mp4";    

    const std::filesystem::path audio_file = assets_path / "audio.pcm";
    const std::filesystem::path subtitle_file = assets_path / "subtitles.srt";
    const std::filesystem::path output_video = assets_path / "output_with_subtitles_turbo.mp4";
    const std::filesystem::path model_path = exe_path / "models" / "ggml-large-v3-turbo-q5_0.bin";

    std::cout <<"pwd: " << exe_path << std::endl << "Assests path: " << assets_path << std::endl << "Model path: "<< model_path << std::endl <<  "Input file: " << input_video << std::endl << "subtitle file: " << subtitle_file << std::endl;
    if (!extractAudio(input_video, audio_file)){
        std::cout << "Failed to generate formated audio file" << std::endl;
    }
    if (transcribe_audio(model_path, audio_file, subtitle_file) != 0){
        std::cout << "Failed to generate subtitles" << std::endl;
    }
    //if (add_subtitles(input_video, subtitle_file, output_video) != 0) return -1;

    return 0;
}


