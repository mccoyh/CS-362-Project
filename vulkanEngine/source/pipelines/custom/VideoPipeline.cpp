#include "VideoPipeline.h"
#include "GraphicsPipelineStates.h"
#include "../RenderPass.h"
#include "../../components/LogicalDevice.h"
#include "../../components/PhysicalDevice.h"
#include <stdexcept>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace VkEngine {
  VideoPipeline::VideoPipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice,
                               const std::shared_ptr<LogicalDevice>& logicalDevice,
                               const std::shared_ptr<RenderPass>& renderPass)
    : GraphicsPipeline(physicalDevice, logicalDevice)
  {
    createPipeline(renderPass->getRenderPass());

    createDescriptorPool();
  }

  VideoPipeline::~VideoPipeline()
  {
    vkDestroyDescriptorPool(logicalDevice->getDevice(), descriptorPool, nullptr);
  }

  void VideoPipeline::render(const VkCommandBuffer& commandBuffer, const VkExtent2D swapChainExtent) const
  {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    const VkViewport viewport {
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(swapChainExtent.width),
      .height = static_cast<float>(swapChainExtent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    const VkRect2D scissor {
      .offset = {0, 0},
      .extent = swapChainExtent
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  }

  void VideoPipeline::loadGraphicsShaders()
  {
    createShader("assets/shaders/video.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    createShader("assets/shaders/video.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
  }

  void VideoPipeline::defineStates()
  {
    defineColorBlendState(GraphicsPipelineStates::colorBlendState);
    defineDepthStencilState(GraphicsPipelineStates::depthStencilStateNone);
    defineDynamicState(GraphicsPipelineStates::dynamicState);
    defineInputAssemblyState(GraphicsPipelineStates::inputAssemblyStateTriangleList);
    defineMultisampleState(GraphicsPipelineStates::getMultsampleState(physicalDevice));
    defineRasterizationState(GraphicsPipelineStates::rasterizationStateNoCull);
    defineVertexInputState(GraphicsPipelineStates::vertexInputStateNone);
    defineViewportState(GraphicsPipelineStates::viewportState);
  }

  void VideoPipeline::createDescriptorPool()
  {
    const std::array<VkDescriptorPoolSize, 11> poolSizes {
      {
        {VK_DESCRIPTOR_TYPE_SAMPLER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MAX_FRAMES_IN_FLIGHT},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, MAX_FRAMES_IN_FLIGHT}
      }};

    const VkDescriptorPoolCreateInfo poolCreateInfo {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets = MAX_FRAMES_IN_FLIGHT,
      .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
      .pPoolSizes = poolSizes.data()
    };

    if (vkCreateDescriptorPool(logicalDevice->getDevice(), &poolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor pool!");
    }
  }
}
