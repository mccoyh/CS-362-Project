#include "VideoDecoder.h"
#include <source/VulkanEngine.h>
#include <iostream>
#include <chrono>

int main()
{
  try
  {
    constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 1280,
      .WINDOW_HEIGHT = 720 + 70,
      .WINDOW_TITLE = "Video Decoding"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

    const VideoDecoder decoder("assets/sample.mp4");

    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    std::chrono::time_point<std::chrono::steady_clock> previousTime = std::chrono::steady_clock::now();
    constexpr float fixedUpdateDt = 1.0f / 30.0f;
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
