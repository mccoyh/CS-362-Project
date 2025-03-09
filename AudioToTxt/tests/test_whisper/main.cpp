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
    const std::filesystem::path modelPath = exePath / "models" / "ggml-large-v3-turbo-q5_0.bin"; //"ggml-base.bin"

    std::cout <<"pwd: " << exePath << std::endl 
        << "Assests path: " << assetsPath << std::endl 
        << "Model path: "<< modelPath << std::endl 
        <<  "Input file: " << inputVideo << std::endl 
        << "subtitle file: " << subtitleFile << std::endl;
    if (!extractAudio(inputVideo.string(), audioFile.string())){
        std::cout << "Failed to generate formated audio file" << std::endl;
    }
    if (Captions::transcribeAudio(modelPath.string(), audioFile.string(), subtitleFile.string()) != 0){
        std::cout << "Failed to generate subtitles" << std::endl;
    }
    int frame = 200; 
    std::string caption = Captions::getSubtitleForFrame(frame, subtitleFile.string());
    if (caption.at(0) == '['){
        std::cout << "Failed to find subtitle at frame. message: "<< caption << std::endl;
    }
    else{
        std::cout << "Caption at frame " << frame << ": " << caption << std::endl;
    }

    Captions::CaptionCache cache(subtitleFile.string());

    std::string captionFromCache = cache.getCaptionAtFrame(frame);
    std::cout << "Cached caption at frame " << frame << ": " << captionFromCache << std::endl;
    std::cout << "Number of captions: " << cache.getNumCaptions() << std::endl;

    return 0;
}


