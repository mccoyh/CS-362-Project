#include "VideoPipeline.h"
#include "GraphicsPipelineStates.h"
#include "../RenderPass.h"
#include "../UniformBuffer.h"
#include "../../components/LogicalDevice.h"
#include "../../components/PhysicalDevice.h"
#include <stdexcept>
#include <array>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace VkEngine {
  VideoPipeline::VideoPipeline(const std::shared_ptr<PhysicalDevice>& physicalDevice,
                               const std::shared_ptr<LogicalDevice>& logicalDevice,
                               const std::shared_ptr<RenderPass>& renderPass)
    : GraphicsPipeline(physicalDevice, logicalDevice)
  {
    createUniforms();

    createDescriptorSetLayout();

    createDescriptorPool();

    createDescriptorSets();

    createPipeline(renderPass->getRenderPass());
  }

  VideoPipeline::~VideoPipeline()
  {
    vkDestroyDescriptorPool(logicalDevice->getDevice(), descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(logicalDevice->getDevice(), descriptorSetLayout, nullptr);
  }

  void VideoPipeline::render(const VkCommandBuffer& commandBuffer, const VkExtent2D swapChainExtent,
                             const VkDescriptorImageInfo* imageInfo,  const uint32_t currentFrame,
                             const float imageAspectRatio) const
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

    const ScreenSizeUniform screenSizeUBO {
      .width = static_cast<float>(swapChainExtent.width),
      .height = static_cast<float>(swapChainExtent.height),
      .imageAspectRatio = imageAspectRatio
    };
    screenSizeUniform->update(currentFrame, &screenSizeUBO, sizeof(ScreenSizeUniform));

    const std::array<VkWriteDescriptorSet, 1> descriptorWrites{{
      {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSets[currentFrame],
        .dstBinding = 1,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = imageInfo
      }
    }};

    vkUpdateDescriptorSets(logicalDevice->getDevice(), descriptorWrites.size(),
                           descriptorWrites.data(), 0, nullptr);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &descriptorSets[currentFrame], 0, nullptr);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  }

  void VideoPipeline::loadGraphicsShaders()
  {
    createShader("assets/shaders/video.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    createShader("assets/shaders/video.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
  }

  void VideoPipeline::loadGraphicsDescriptorSetLayouts()
  {
    loadDescriptorSetLayout(descriptorSetLayout);
  }

  void VideoPipeline::createDescriptorSetLayout()
  {
    constexpr VkDescriptorSetLayoutBinding textureLayout {
      .binding = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    constexpr VkDescriptorSetLayoutBinding screenSizeLayout {
      .binding = 2,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    constexpr std::array objectBindings {
      textureLayout,
      screenSizeLayout
    };

    const VkDescriptorSetLayoutCreateInfo objectLayoutCreateInfo {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(objectBindings.size()),
      .pBindings = objectBindings.data()
    };

    if (vkCreateDescriptorSetLayout(logicalDevice->getDevice(), &objectLayoutCreateInfo, nullptr,
                                    &descriptorSetLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create descriptor set layout!");
    }
  }

  void VideoPipeline::createDescriptorSets()
  {
    const std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    const VkDescriptorSetAllocateInfo allocateInfo {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = descriptorPool,
      .descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
      .pSetLayouts = layouts.data()
    };

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(logicalDevice->getDevice(), &allocateInfo, descriptorSets.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      std::array<VkWriteDescriptorSet, 1> descriptorWrites{{
        screenSizeUniform->getDescriptorSet(2, descriptorSets[i], i)
      }};

      vkUpdateDescriptorSets(logicalDevice->getDevice(), descriptorWrites.size(),
                             descriptorWrites.data(), 0, nullptr);
    }
  }

  void VideoPipeline::createUniforms()
  {
    screenSizeUniform = std::make_unique<UniformBuffer>(logicalDevice, physicalDevice, MAX_FRAMES_IN_FLIGHT,
                                                        sizeof(ScreenSizeUniform));
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
