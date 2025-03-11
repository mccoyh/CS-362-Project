#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace VkEngine {

class LogicalDevice;
class PhysicalDevice;

class Pipeline {
public:
  Pipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice, const std::shared_ptr<LogicalDevice>& logicalDevice);

  virtual ~Pipeline();

protected:
  std::shared_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<LogicalDevice> logicalDevice;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
};

} // VkEngine

#endif //PIPELINE_H
