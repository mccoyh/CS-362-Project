#include "VideoDecoder.h"
#include <source/VulkanEngine.h>
#include <iostream>
#include <chrono>

int main(const int argc, char* argv[])
{
  try
  {
    const VideoDecoder decoder(argc == 2 ? argv[1] :"assets/sample_1080.mp4");
    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    const VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = static_cast<uint32_t>(decoder.getWidth()),
      .WINDOW_HEIGHT = static_cast<uint32_t>(decoder.getHeight() + 70),
      .WINDOW_TITLE = "Video Decoding"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

    std::chrono::time_point<std::chrono::steady_clock> previousTime = std::chrono::steady_clock::now();
    const float fixedUpdateDt = 1.0f / static_cast<float>(decoder.getFrameRate());
    float timeAccumulator = 0;

    while (vulkanEngine.isActive())
    {
      const auto currentTime = std::chrono::steady_clock::now();
      const float dt = std::chrono::duration<float>(currentTime - previousTime).count();
      previousTime = currentTime;

      timeAccumulator += dt;
      while (timeAccumulator >= fixedUpdateDt)
      {
        if (decoder.getNextFrame(*frameData, frameWidth, frameHeight))
        {
          vulkanEngine.loadVideoFrame(frameData, frameWidth, frameHeight);
        }

        timeAccumulator -= fixedUpdateDt;
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
