#include "VideoDecoder.h"
#include <source/VulkanEngine.h>
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>

// Keyboard callback function prototype
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Global decoder pointer for use in callback
VideoDecoder* g_decoder = nullptr;

int main()
{
  try
  {
    VideoDecoder decoder("assets/turtles.mp4");
    g_decoder = &decoder;  // Set global pointer for callback
    
    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    const VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = static_cast<uint32_t>(decoder.getWidth()),
      .WINDOW_HEIGHT = static_cast<uint32_t>(decoder.getHeight() + 70),
      .WINDOW_TITLE = "Video Decoding - Press 'Space' to Pause/Resume, 'Left/Right' to Seek, 'R' to Restart"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    
    // Register keyboard callback with GLFW window
    GLFWwindow* window = vulkanEngine.getWindow()->getGlfwWindow();
    if (window) {
      glfwSetKeyCallback(window, keyCallback);
    }

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
    
    g_decoder = nullptr;  // Reset global pointer
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// Keyboard callback implementation
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (g_decoder == nullptr) return;
  
  // Only process key press events (not key release or repeat)
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_SPACE:  // Play/Pause
        if (g_decoder->isPaused()) {
          g_decoder->resume();
          std::cout << "Video resumed" << std::endl;
        } else {
          g_decoder->pause();
          std::cout << "Video paused" << std::endl;
        }
        break;
        
      case GLFW_KEY_RIGHT:  // Forward 5 seconds
        g_decoder->seekForward(5);
        std::cout << "Seeking forward 5 seconds" << std::endl;
        break;
        
      case GLFW_KEY_LEFT:  // Backward 5 seconds
        g_decoder->seekBackward(5);
        std::cout << "Seeking backward 5 seconds" << std::endl;
        break;
        
      case GLFW_KEY_R:  // Restart video
        g_decoder->restart();
        std::cout << "Restarting video" << std::endl;
        break;
        
      case GLFW_KEY_ESCAPE:  // Exit application
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    }
  }
}
