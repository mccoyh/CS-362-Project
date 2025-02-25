#include <source/AudioToTxt.h>
#include <filesystem>
#include <iostream>
#include <cstdlib>

#include "audioDecoding.h"

int main() {
   
    

    std::filesystem::path exePath = std::filesystem::current_path(); // Gets the current working directory (where executable runs)

    // Path to the assets folder relative to the executable's directory
    std::filesystem::path assetsPath =  exePath / "assets";
    std::filesystem::path inputVideo = assetsPath / "CS_test.mp4";    

    const std::filesystem::path audioFile = assetsPath / "audio.pcm";
    const std::filesystem::path subtitleFile = assetsPath / "subtitles.srt";
    const std::filesystem::path outputVideo = assetsPath / "output_with_subtitles_turbo.mp4";
    const std::filesystem::path modelPath = exePath / "models" / "ggml-base.bin";

    std::cout <<"pwd: " << exePath << std::endl << "Assests path: " << assetsPath << std::endl << "Model path: "<< modelPath << std::endl <<  "Input file: " << inputVideo << std::endl << "subtitle file: " << subtitleFile << std::endl;
    if (!extractAudio(inputVideo, audioFile)){
        std::cout << "Failed to generate formated audio file" << std::endl;
    }
    if (transcribeAudio(modelPath, audioFile, subtitleFile) != 0){
        std::cout << "Failed to generate subtitles" << std::endl;
    }
    //if (add_subtitles(inputVideo, subtitleFile, outputVideo) != 0) return -1;

    return 0;
}


