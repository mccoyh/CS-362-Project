#include <audio.h>
#include <AudioToTxt.h>
#include <AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <filesystem>
#include "../AudioToTxt/tests/test_whisper/audioDecoding.h"

constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
  .WINDOW_WIDTH = 1000,
  .WINDOW_HEIGHT = 600,
  .WINDOW_TITLE = "Medos Media Player"
};

int main(const int argc, char* argv[])
{
  try
  {
    const auto asset = argc == 2 ? argv[1] : "assets/CS_test.mp4";

    /* begin captions initialization */

    // Path to the assets folder as a string
    std::string assetsPath = "assets/";

    const std::string audioFile = assetsPath + "audio.pcm";
    const std::string subtitleFile = assetsPath + "subtitles.srt";
    const std::string outputVideo = assetsPath + "output_with_subtitles_turbo.mp4";
    const std::string modelPath = "models/ggml-large-v3-turbo-q5_0.bin"; // "ggml-base.bin"

    std::cout << "Assests path: " << assetsPath << std::endl
        << "Model path: "<< modelPath << std::endl
        << "Input file: " << asset << std::endl
        << "subtitle file: " << subtitleFile << std::endl;

    if (!extractAudio(asset, audioFile))
    {
      std::cout << "Failed to generate formated audio file" << std::endl;
    }

    if (Captions::transcribeAudio(modelPath, audioFile, subtitleFile) != 0)
    {
      std::cout << "Failed to generate subtitles" << std::endl;
    }

    Captions::CaptionCache cache(subtitleFile);

    /* End captions initialization */

    /* begin audio initialization */

    Audio::convertWav(asset, "audio");

    auto parser = AVParser::MediaParser(asset);

    Audio::initSDL();

    // create and play audio stream
    const Audio::AudioData audio = Audio::playAudio("audio.wav");
    uint32_t duration = audio.duration;
    uint32_t change = audio.duration - 10000;
    uint32_t stop = 0;

    /* end audio initialization */

    /* Begin Video initialization */

    const auto frameData = parser.getCurrentFrame();

    std::cout << "Frame Width: " << frameData.frameWidth << "\n"
              << "Frame Height: " << frameData.frameHeight << "\n"
              << "Total Frames: " << parser.getTotalFrames() << std::endl;

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());
    const auto gui = vulkanEngine.getImGuiInstance();

    /* end video initialization */

    while (vulkanEngine.isActive())
    {
      parser.update();

      const std::string captionFromCache = cache.getCaptionAtFrame(parser.getCurrentFrameIndex() / parser.getFrameRate() * 100);

      vulkanEngine.loadCaption(captionFromCache.c_str());

      const auto frame = parser.getCurrentFrame();

      vulkanEngine.loadVideoFrame(frame.videoData, frame.frameWidth, frame.frameHeight);

      vulkanEngine.render();

      if (duration > 0)
      {
        Audio::delay(10); // checks for extra input every 10 ms

        duration -= 10;
      }
    }

    Audio::deleteStream(audio.stream);
    Audio::quitSDL();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}