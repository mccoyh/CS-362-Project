#include "VideoDecoder.h"
#include <source/VulkanEngine.h>
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>

// Global decoder pointer for use in callback
VideoDecoder* g_decoder = nullptr;

int main(const int argc, char* argv[])
{
  try
  {
    VideoDecoder decoder(argc == 2 ? argv[1] : "assets/CS_test.mp4");
    g_decoder = &decoder;  // Set global pointer for callback

    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    const VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = static_cast<uint32_t>(decoder.getWidth()),
      .WINDOW_HEIGHT = static_cast<uint32_t>(decoder.getHeight() + 70),
      .WINDOW_TITLE = "Video Decoding - Press 'Space' to Pause/Resume,'R' to Restart"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

    std::chrono::time_point<std::chrono::steady_clock> previousTime = std::chrono::steady_clock::now();
    const float fixedUpdateDt = 1.0f / static_cast<float>(decoder.getFrameRate());
    float timeAccumulator = 0;

    // For debouncing key presses
    bool spaceWasPressed = false;
    bool rightWasPressed = false;
    bool leftWasPressed = false;
    bool rWasPressed = false;

    while (vulkanEngine.isActive())
    {
      const auto currentTime = std::chrono::steady_clock::now();
      const float dt = std::chrono::duration<float>(currentTime - previousTime).count();
      previousTime = currentTime;

      // Handle keyboard input using keyIsPressed
      bool spaceIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_SPACE);
      if (spaceIsPressed && !spaceWasPressed) {
        if (decoder.isPaused()) {
          decoder.resume();
          std::cout << "Video resumed" << std::endl;
        } else {
          decoder.pause();
          std::cout << "Video paused" << std::endl;
        }
      }
      spaceWasPressed = spaceIsPressed;

      bool rightIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_RIGHT);
      if (rightIsPressed && !rightWasPressed) {
        decoder.seekForward(5);
        std::cout << "Seeking forward 5 seconds" << std::endl;
      }
      rightWasPressed = rightIsPressed;

      bool leftIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_LEFT);
      if (leftIsPressed && !leftWasPressed) {
        decoder.seekBackward(5);
        std::cout << "Seeking backward 5 seconds" << std::endl;
      }
      leftWasPressed = leftIsPressed;

      bool rIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_R);
      if (rIsPressed && !rWasPressed) {
        decoder.restart();
        std::cout << "Restarting video" << std::endl;
      }
      rWasPressed = rIsPressed;

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

    g_decoder = nullptr;  // Reset global pointer
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
