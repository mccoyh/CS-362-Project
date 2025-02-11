#include "VulkanEngine.h"
#include "components/Instance.h"
#include "components/Window.h"
#include "components/DebugMessenger.h"
#include "components/LogicalDevice.h"
#include "components/PhysicalDevice.h"
#include "components/SwapChain.h"
#include "pipelines/RenderPass.h"
#include <stdexcept>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace VkEngine {

  constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  VulkanEngine::VulkanEngine()
    : currentFrame(0), framebufferResized(false)
  {
    glfwInit();

    initVulkan();
  }

  VulkanEngine::~VulkanEngine()
  {
    logicalDevice->waitIdle();

    vkDestroyCommandPool(logicalDevice->getDevice(), commandPool, nullptr);

    glfwTerminate();
  }

  bool VulkanEngine::isActive() const
  {
    return window->isOpen();
  }

  void VulkanEngine::render()
  {
    window->update();

    doRendering();
  }

  void VulkanEngine::initVulkan()
  {
    instance = std::make_shared<Instance>();

    if (enableValidationLayers)
    {
      debugMessenger = std::make_unique<DebugMessenger>(instance);
    }

    window = std::make_shared<Window>(600, 400, "Vulkan Engine", instance, false);

    physicalDevice = std::make_shared<PhysicalDevice>(instance, window->getSurface());

    logicalDevice = std::make_shared<LogicalDevice>(physicalDevice);

    createCommandPool();
    allocateCommandBuffers(swapchainCommandBuffers);

    swapChain = std::make_shared<SwapChain>(physicalDevice, logicalDevice, window);

    renderPass = std::make_shared<RenderPass>(logicalDevice, physicalDevice, swapChain->getImageFormat(),
                                              physicalDevice->getMsaaSamples(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  }

  void VulkanEngine::createCommandPool()
  {
    const auto queueFamilyIndices = physicalDevice->getQueueFamilies();

    const VkCommandPoolCreateInfo poolInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
    };

    if (vkCreateCommandPool(logicalDevice->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create command pool!");
    }
  }

  void VulkanEngine::allocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers) const
  {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    const VkCommandBufferAllocateInfo allocInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = static_cast<uint32_t>(commandBuffers.size())
    };

    if (vkAllocateCommandBuffers(logicalDevice->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void VulkanEngine::recordCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex,
                                         const std::function<void(const VkCommandBuffer& cmdBuffer, uint32_t imgIndex)>& renderFunction)
  {
    constexpr VkCommandBufferBeginInfo beginInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    renderFunction(commandBuffer, imageIndex);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void VulkanEngine::recordSwapchainCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32_t imageIndex) const
  {
    recordCommandBuffer(commandBuffer, imageIndex, [this](const VkCommandBuffer& cmdBuffer,
                        const uint32_t imgIndex)
    {
      // TODO
    });
  }

  void VulkanEngine::doRendering()
  {
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }
} // VkEngine