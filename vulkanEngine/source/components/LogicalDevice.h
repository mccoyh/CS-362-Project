#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace VkEngine {

class PhysicalDevice;

class LogicalDevice {
public:
  explicit LogicalDevice(const std::shared_ptr<PhysicalDevice>& physicalDevice);
  ~LogicalDevice();

  [[nodiscard]] VkDevice getDevice() const;
  void waitIdle() const;

  [[nodiscard]] VkQueue getGraphicsQueue() const;
  [[nodiscard]] VkQueue getPresentQueue() const;

  void submitGraphicsQueue(uint32_t currentFrame, const VkCommandBuffer* commandBuffer) const;
  void submitVideoGraphicsQueue(uint32_t currentFrame, const VkCommandBuffer* commandBuffer) const;

  void waitForGraphicsFences(uint32_t currentFrame) const;
  void resetGraphicsFences(uint32_t currentFrame) const;

  VkResult queuePresent(uint32_t currentFrame, const VkSwapchainKHR& swapchain, const uint32_t* imageIndex) const;

  VkResult acquireNextImage(uint32_t currentFrame, const VkSwapchainKHR& swapchain, uint32_t* imageIndex) const;

private:
  VkDevice device = VK_NULL_HANDLE;

  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentQueue = VK_NULL_HANDLE;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  std::vector<VkSemaphore> videoImageAvailableSemaphores;
  std::vector<VkSemaphore> videoRenderFinishedSemaphores;
  std::vector<VkFence> videoInFlightFences;

  void createDevice(const std::shared_ptr<PhysicalDevice>& physicalDevice);

  void createSyncObjects();

  void destroySyncObjects() const;
};

} // VkEngine

#endif //LOGICALDEVICE_H
