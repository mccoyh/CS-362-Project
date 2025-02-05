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

  VkDevice& getDevice();
  void waitIdle() const;

  VkQueue& getGraphicsQueue();
  VkQueue& getPresentQueue();

  void submitGraphicsQueue(uint32_t currentFrame, const VkCommandBuffer* commandBuffer) const;

  void waitForGraphicsFences(uint32_t currentFrame) const;
  void resetGraphicsFences(uint32_t currentFrame) const;

  VkResult queuePresent(uint32_t currentFrame, const VkSwapchainKHR& swapchain, const uint32_t* imageIndex) const;

  VkResult acquireNextImage(uint32_t currentFrame, const VkSwapchainKHR& swapchain, uint32_t* imageIndex) const;

private:
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  void createDevice(const std::shared_ptr<PhysicalDevice>& physicalDevice);

  void createSyncObjects();
};

} // VkEngine

#endif //LOGICALDEVICE_H
