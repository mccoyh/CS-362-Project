#ifndef VIDEOPIPELINE_H
#define VIDEOPIPELINE_H

#include "source/pipelines/GraphicsPipeline.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace VkEngine {

class RenderPass;
class UniformBuffer;

struct ScreenSizeUniform {
  float width;
  float height;
  float imageAspectRatio;
  int grayscale;
};

class VideoPipeline final : public GraphicsPipeline {
public:
  VideoPipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice,
                const std::shared_ptr<LogicalDevice>& logicalDevice,
                const std::shared_ptr<RenderPass>& renderPass);

  ~VideoPipeline() override;

  void render(const VkCommandBuffer& commandBuffer, VkExtent2D swapChainExtent, const VkDescriptorImageInfo* imageInfo,
              uint32_t currentFrame, float imageAspectRatio, bool grayscale) const;

private:
  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> descriptorSets;

  VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

  std::unique_ptr<UniformBuffer> screenSizeUniform;

  void loadGraphicsShaders() override;

  void loadGraphicsDescriptorSetLayouts() override;

  void defineStates() override;

  void createDescriptorPool();

  void createDescriptorSetLayout();

  void createDescriptorSets();

  void createUniforms();
};

}

#endif //VIDEOPIPELINE_H
