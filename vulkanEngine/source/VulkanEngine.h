#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <memory>

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

  void doRendering();
};

} // VkEngine

#endif //VULKANENGINE_H
