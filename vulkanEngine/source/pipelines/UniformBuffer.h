#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace VkEngine {

class LogicalDevice;
class PhysicalDevice;

class UniformBuffer {
public:
  UniformBuffer(const std::shared_ptr<LogicalDevice>& logicalDevice,
                const std::shared_ptr<PhysicalDevice>& physicalDevice,
                uint32_t MAX_FRAMES_IN_FLIGHT, VkDeviceSize bufferSize);
  ~UniformBuffer();

  [[nodiscard]] VkDescriptorPoolSize getDescriptorPoolSize() const;

  [[nodiscard]] VkWriteDescriptorSet getDescriptorSet(uint32_t binding, const VkDescriptorSet& dstSet, size_t frame) const;

  void update(uint32_t frame, const void* data, size_t size) const;

protected:
  std::shared_ptr<LogicalDevice> logicalDevice;

  uint32_t MAX_FRAMES_IN_FLIGHT;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void*> uniformBuffersMapped;

  std::vector<VkDescriptorBufferInfo> bufferInfos;
  VkDescriptorPoolSize poolSize{};
};

} // VkEngine

#endif //UNIFORMBUFFER_H
