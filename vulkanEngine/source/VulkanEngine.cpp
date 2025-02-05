#include "VulkanEngine.h"
#include "components/Instance.h"
#include "components/Window.h"
#include "components/DebugMessenger.h"
#include "components/PhysicalDevice.h"

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace VkEngine {
  VulkanEngine::VulkanEngine()
  {
    glfwInit();

    instance = std::make_unique<Instance>();

    if (enableValidationLayers)
    {
      debugMessenger = std::make_unique<DebugMessenger>(instance->getInstance());
    }

    window = std::make_shared<Window>(600, 400, "Vulkan Engine", instance->getInstance(), false);

    physicalDevice = std::make_shared<PhysicalDevice>(instance->getInstance(), window->getSurface());
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