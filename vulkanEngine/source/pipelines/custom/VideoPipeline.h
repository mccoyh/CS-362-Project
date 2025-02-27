#ifndef VIDEOPIPELINE_H
#define VIDEOPIPELINE_H

#include "source/pipelines/GraphicsPipeline.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace VkEngine {

class RenderPass;

class VideoPipeline final : public GraphicsPipeline {
public:
  VideoPipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice,
                const std::shared_ptr<LogicalDevice>& logicalDevice,
                const std::shared_ptr<RenderPass>& renderPass);

  ~VideoPipeline() override;

  void render(const VkCommandBuffer& commandBuffer, VkExtent2D swapChainExtent, const VkDescriptorImageInfo* imageInfo,
              uint32_t currentFrame) const;

private:
  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> descriptorSets;

  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

  void loadGraphicsShaders() override;

  void loadGraphicsDescriptorSetLayouts() override;

  void defineStates() override;

  void createDescriptorPool();

  void createDescriptorSetLayout();

  void createDescriptorSets();
};

}

#endif //VIDEOPIPELINE_H
