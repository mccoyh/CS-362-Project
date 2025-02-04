#include "VulkanEngine.h"
#include "components/Instance.h"
#include "components/Window.h"

namespace VkEngine {
  VulkanEngine::VulkanEngine()
  {
    glfwInit();

    instance = std::make_unique<Instance>();

    window = std::make_shared<Window>(600, 400, "Vulkan Engine", instance->getInstance(), false);
  }

  VulkanEngine::~VulkanEngine()
  {
    glfwTerminate();
  }

  bool VulkanEngine::isActive() const
  {
    return window->isOpen();
  }

  void VulkanEngine::render()
  {
    window->update();
  }
} // VkEngine