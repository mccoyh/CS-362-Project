#include "../VideoDecoder.h"
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <chrono>

int main(const int argc, char* argv[])
{
  try
  {
    const VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 600,
      .WINDOW_HEIGHT = 400,
      .WINDOW_TITLE = "Special Effects"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());
    const auto gui = vulkanEngine.getImGuiInstance();

    const VideoDecoder decoder(argc == 2 ? argv[1] : "assets/CS_test.mp4");
    const auto frameData = std::make_shared<std::vector<uint8_t>>();
    int frameWidth, frameHeight;

    std::chrono::time_point<std::chrono::steady_clock> previousTime = std::chrono::steady_clock::now();
    const float fixedUpdateDt = 1.0f / static_cast<float>(decoder.getFrameRate());
    float timeAccumulator = 0;

    bool grayscale = false;

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

      gui->dockBottom("Special Effects");

      gui->setBottomDockPercent(0.3);

      ImGui::Begin("Special Effects");

      ImGui::Checkbox("Grayscale", &grayscale);

      ImGui::End();

      vulkanEngine.setGrayscale(grayscale);

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
