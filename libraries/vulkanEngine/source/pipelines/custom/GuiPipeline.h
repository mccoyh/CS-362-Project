#ifndef GUIPIPELINE_H
#define GUIPIPELINE_H

#include "../GraphicsPipeline.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace VkEngine {

class RenderPass;

class GuiPipeline final : public GraphicsPipeline {
public:
  GuiPipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice,
              const std::shared_ptr<LogicalDevice>& logicalDevice,
              const std::shared_ptr<RenderPass>& renderPass,
              uint32_t maxImGuiTextures);

  ~GuiPipeline() override;

  void render(const VkCommandBuffer& commandBuffer, VkExtent2D swapChainExtent) const;

  [[nodiscard]] VkDescriptorPool getPool() const;

private:
  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

  void loadGraphicsShaders() override;

  void defineStates() override;

  void createDescriptorPool(uint32_t maxImGuiTextures);
};

} // VkEngine

#endif //GUIPIPELINE_H
