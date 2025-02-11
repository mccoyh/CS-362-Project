#include "VulkanEngine.h"
#include "components/Instance.h"
#include "components/Window.h"
#include "components/DebugMessenger.h"
#include "components/LogicalDevice.h"
#include "components/PhysicalDevice.h"
#include "components/SwapChain.h"
#include "pipelines/RenderPass.h"

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace VkEngine {
  VulkanEngine::VulkanEngine()
  {
    glfwInit();

    instance = std::make_shared<Instance>();

    if (enableValidationLayers)
    {
      debugMessenger = std::make_unique<DebugMessenger>(instance);
    }

    window = std::make_shared<Window>(600, 400, "Vulkan Engine", instance, false);

    physicalDevice = std::make_shared<PhysicalDevice>(instance, window->getSurface());

    logicalDevice = std::make_shared<LogicalDevice>(physicalDevice);

    swapChain = std::make_shared<SwapChain>(physicalDevice, logicalDevice, window);

    renderPass = std::make_shared<RenderPass>(logicalDevice, physicalDevice, swapChain->getImageFormat(),
                                              physicalDevice->getMsaaSamples(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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