#include <source/VulkanEngine.h>
#include <iostream>

int main()
{
  try
  {
    constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 600,
      .WINDOW_HEIGHT = 400,
      .WINDOW_TITLE = "Window Test"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);

    while (vulkanEngine.isActive())
    {
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