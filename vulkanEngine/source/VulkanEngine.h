#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace VkEngine {
class Instance;
class DebugMessenger;
class Window;
class PhysicalDevice;
class LogicalDevice;
class SwapChain;
class RenderPass;

class VulkanEngine {
public:
  VulkanEngine();
  ~VulkanEngine();

  [[nodiscard]] bool isActive() const;

  void render();

private:
  std::shared_ptr<Instance> instance;
  std::unique_ptr<DebugMessenger> debugMessenger;
  std::shared_ptr<Window> window;
  std::shared_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<LogicalDevice> logicalDevice;

  std::shared_ptr<SwapChain> swapChain;
  std::shared_ptr<RenderPass> renderPass;

  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> swapchainCommandBuffers;

  uint32_t currentFrame;
  bool framebufferResized;

  void initVulkan();
  void createCommandPool();
  void allocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers) const;

  void doRendering();
};

} // VkEngine

#endif //VULKANENGINE_H
