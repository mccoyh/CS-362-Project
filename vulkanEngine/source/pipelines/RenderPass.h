#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace VkEngine {

class LogicalDevice;
class PhysicalDevice;

class RenderPass {
public:
  RenderPass(const std::shared_ptr<LogicalDevice>& logicalDevice,
             const std::shared_ptr<PhysicalDevice>& physicalDevice, VkFormat imageFormat,
             VkSampleCountFlagBits msaaSamples, VkImageLayout finalLayout);
  ~RenderPass();

  [[nodiscard]] VkRenderPass getRenderPass() const;

  [[nodiscard]] VkFormat findDepthFormat() const;

  void begin(const VkFramebuffer& framebuffer, const VkExtent2D& extent, const VkCommandBuffer& commandBuffer) const;
  static void end(const VkCommandBuffer& commandBuffer);

private:
  std::shared_ptr<LogicalDevice> logicalDevice;
  std::shared_ptr<PhysicalDevice> physicalDevice;

  VkRenderPass renderPass = VK_NULL_HANDLE;

  void createRenderPass(VkFormat imageFormat, VkSampleCountFlagBits msaaSamples, VkImageLayout finalLayout);

  [[nodiscard]] VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                             VkFormatFeatureFlags features) const;
};

} // VkEngine

#endif //RENDERPASS_H
