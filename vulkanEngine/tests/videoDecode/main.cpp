#include "VideoDecoder.h"
#include <source/VulkanEngine.h>
#include <iostream>

int main()
{
  try
  {
    constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 600,
      .WINDOW_HEIGHT = 400,
      .WINDOW_TITLE = "Video Decoding"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

    const VideoDecoder decoder("assets/sample.mp4");

    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    while (vulkanEngine.isActive())
    {
      if (decoder.getNextFrame(*frameData, frameWidth, frameHeight))
      {
        vulkanEngine.loadVideoFrame(frameData, frameWidth, frameHeight);
      }

      vulkanEngine.render();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
