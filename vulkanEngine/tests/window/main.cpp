#include <source/VulkanEngine.h>

int main()
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