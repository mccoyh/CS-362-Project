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

  void render(const VkCommandBuffer& commandBuffer, VkExtent2D swapChainExtent) const;

private:
  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

  void loadGraphicsShaders() override;

  void defineStates() override;

  void createDescriptorPool();
};

}

#endif //VIDEOPIPELINE_H
