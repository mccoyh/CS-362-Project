#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include "VulkanEngineOptions.h"
#include "components/Window.h"
#include <imgui_internal.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <functional>

namespace VkEngine {
class Instance;
class DebugMessenger;
class Window;
class PhysicalDevice;
class LogicalDevice;
class SwapChain;
class RenderPass;
class Framebuffer;
class GuiPipeline;
class ImGuiInstance;

class VulkanEngine {
public:
  explicit VulkanEngine(const VulkanEngineOptions& vulkanEngineOptions);
  ~VulkanEngine();

  [[nodiscard]] bool isActive() const;

  void render();

  [[nodiscard]] std::shared_ptr<ImGuiInstance> getImGuiInstance() const;

  static ImGuiContext* getImGuiContext();

  void loadVideoFrame(std::shared_ptr<std::vector<uint8_t>> frameData, int width, int height);

private:
  VulkanEngineOptions vulkanEngineOptions;

  std::shared_ptr<Instance> instance;
  std::unique_ptr<DebugMessenger> debugMessenger;
  std::shared_ptr<Window> window;
  std::shared_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<LogicalDevice> logicalDevice;

  std::shared_ptr<SwapChain> swapChain;
  std::shared_ptr<RenderPass> renderPass;

  std::unique_ptr<GuiPipeline> guiPipeline;

  std::shared_ptr<ImGuiInstance> imGuiInstance;

  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> swapchainCommandBuffers;

  std::shared_ptr<Framebuffer> framebuffer;

  uint32_t currentFrame;
  bool framebufferResized;

  std::shared_ptr<RenderPass> videoRenderPass;
  std::shared_ptr<Framebuffer> videoFramebuffer;
  std::vector<VkCommandBuffer> videoCommandBuffers;
  VkExtent2D videoExtent{};

  std::shared_ptr<std::vector<uint8_t>> videoFrameData;

  void initVulkan();
  void createCommandPool();
  void allocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers) const;

  static void recordCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex,
                                  const std::function<void(const VkCommandBuffer& cmdBuffer, uint32_t imgIndex)>& renderFunction);

  void recordSwapchainCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex) const;

  void recordVideoCommandBuffer(const VkCommandBuffer& commandBuffer, uint32_t imageIndex) const;

  void doRendering();

  void recreateSwapChain();

  void createNewFrame() const;

  void renderVideoWidget(uint32_t imageIndex) const;

  void loadVideoFrameToImage(int framebufferIndex) const;

  friend void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

} // VkEngine

#endif //VULKANENGINE_H
