#include <source/VulkanEngine.h>

int main()
{
  auto vulkanEngine = VkEngine::VulkanEngine();

  while (vulkanEngine.isActive())
  {
    vulkanEngine.render();
  }
}