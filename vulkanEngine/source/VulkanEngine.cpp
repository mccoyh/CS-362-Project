#include "VulkanEngine.h"
#include "components/Instance.h"
#include "components/Window.h"
#include "components/DebugMessenger.h"
#include "components/LogicalDevice.h"
#include "components/PhysicalDevice.h"
#include "components/SwapChain.h"
#include "components/Framebuffer.h"
#include "components/ImGuiInstance.h"
#include "pipelines/RenderPass.h"
#include "pipelines/custom/GuiPipeline.h"
#include "pipelines/custom/VideoPipeline.h"
#include "utilities/Buffers.h"
#include "utilities/Images.h"
#include <stdexcept>
#include <utility>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr int MAX_GUI_TEXTURES = 1000;

namespace VkEngine {

  constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  VulkanEngine::VulkanEngine(const VulkanEngineOptions& vulkanEngineOptions)
    : vulkanEngineOptions(vulkanEngineOptions), currentFrame(0), framebufferResized(false)
  {
    glfwInit();

    initVulkan();

    setupVideoTexture();
  }

  VulkanEngine::~VulkanEngine()
  {
    logicalDevice->waitIdle();

    destroyVideoTexture();

    vkDestroyCommandPool(logicalDevice->getDevice(), commandPool, nullptr);

    glfwTerminate();
  }

  bool VulkanEngine::isActive() const
  {
    return window->isOpen();
  }

  void VulkanEngine::render()
  {
    window->update();

    doRendering();

    createNewFrame();
  }

  std::shared_ptr<ImGuiInstance> VulkanEngine::getImGuiInstance() const
  {
    return imGuiInstance;
  }

  ImGuiContext* VulkanEngine::getImGuiContext()
  {
    return ImGui::GetCurrentContext();
  }

  void VulkanEngine::loadVideoFrame(std::shared_ptr<std::vector<uint8_t>> frameData, const int width, const int height)
  {
    videoFrameData = std::move(frameData);

    if (videoExtent.width != width || videoExtent.height != height)
    {
      videoExtent.width = width;
      videoExtent.height = height;

      destroyVideoTexture();
      setupVideoTexture();

      logicalDevice->waitIdle();
      videoFramebuffer.reset();
      videoFramebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, nullptr, commandPool,
                                                       videoRenderPass, videoExtent);
    }
  }

  void VulkanEngine::initVulkan()
  {
    instance = std::make_shared<Instance>();

    if (enableValidationLayers)
    {
      debugMessenger = std::make_unique<DebugMessenger>(instance);
    }

    window = std::make_shared<Window>(vulkanEngineOptions.WINDOW_WIDTH, vulkanEngineOptions.WINDOW_HEIGHT,
                                      vulkanEngineOptions.WINDOW_TITLE, instance, vulkanEngineOptions.FULLSCREEN);

    physicalDevice = std::make_shared<PhysicalDevice>(instance, window->getSurface());

    logicalDevice = std::make_shared<LogicalDevice>(physicalDevice);

    createCommandPool();
    allocateCommandBuffers(swapchainCommandBuffers);
    allocateCommandBuffers(videoCommandBuffers);

    swapChain = std::make_shared<SwapChain>(physicalDevice, logicalDevice, window);

    renderPass = std::make_shared<RenderPass>(logicalDevice, physicalDevice, swapChain->getImageFormat(),
                                              physicalDevice->getMsaaSamples(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);


    framebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, swapChain, commandPool, renderPass,
                                                swapChain->getExtent());

    guiPipeline = std::make_unique<GuiPipeline>(physicalDevice, logicalDevice, renderPass, MAX_GUI_TEXTURES);


    imGuiInstance = std::make_shared<ImGuiInstance>(commandPool, window, instance, physicalDevice, logicalDevice,
                                                    renderPass, guiPipeline, true);

    videoRenderPass = std::make_shared<RenderPass>(logicalDevice, physicalDevice, VK_FORMAT_R8G8B8A8_UNORM,
                                                   physicalDevice->getMsaaSamples(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    videoFramebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, nullptr, commandPool,
                                                     videoRenderPass, swapChain->getExtent());

    videoPipeline = std::make_unique<VideoPipeline>(physicalDevice, logicalDevice, videoRenderPass);
  }

  void VulkanEngine::createCommandPool()
  {
    const auto queueFamilyIndices = physicalDevice->getQueueFamilies();

    const VkCommandPoolCreateInfo poolInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value()
    };

    if (vkCreateCommandPool(logicalDevice->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create command pool!");
    }
  }

  void VulkanEngine::allocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers) const
  {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    const VkCommandBufferAllocateInfo allocInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = static_cast<uint32_t>(commandBuffers.size())
    };

    if (vkAllocateCommandBuffers(logicalDevice->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void VulkanEngine::recordCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32_t imageIndex,
                                         const std::function<void(const VkCommandBuffer& cmdBuffer, uint32_t imgIndex)>& renderFunction)
  {
    constexpr VkCommandBufferBeginInfo beginInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    renderFunction(commandBuffer, imageIndex);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void VulkanEngine::recordSwapchainCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32_t imageIndex) const
  {
    recordCommandBuffer(commandBuffer, imageIndex, [this](const VkCommandBuffer& cmdBuffer,
                        const uint32_t imgIndex)
    {
      renderPass->begin(framebuffer->getFramebuffer(imgIndex), swapChain->getExtent(), cmdBuffer);

      guiPipeline->render(cmdBuffer, swapChain->getExtent());

      RenderPass::end(cmdBuffer);
    });
  }

  void VulkanEngine::recordVideoCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32_t imageIndex) const
  {
    recordCommandBuffer(commandBuffer, imageIndex, [this](const VkCommandBuffer& cmdBuffer,
                        const uint32_t imgIndex)
    {
      if (videoExtent.width == 0 || videoExtent.height == 0)
      {
        return;
      }

      if (videoFrameData)
      {
        loadVideoFrameToImage(static_cast<int>(imgIndex));
      }

      videoRenderPass->begin(videoFramebuffer->getFramebuffer(imgIndex), videoExtent, cmdBuffer);

      videoPipeline->render(cmdBuffer, videoExtent, &videoTextureImageInfos[currentFrame], currentFrame);

      RenderPass::end(cmdBuffer);
    });
  }

  void VulkanEngine::doRendering()
  {
    logicalDevice->waitForGraphicsFences(currentFrame);

    uint32_t imageIndex;
    auto result = logicalDevice->acquireNextImage(currentFrame, swapChain->getSwapChain(), &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      framebufferResized = false;
      recreateSwapChain();
      return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    renderVideoWidget(imageIndex);

    logicalDevice->resetGraphicsFences(currentFrame);

    vkResetCommandBuffer(videoCommandBuffers[currentFrame], 0);
    recordVideoCommandBuffer(videoCommandBuffers[currentFrame], imageIndex);
    logicalDevice->submitVideoGraphicsQueue(currentFrame, &videoCommandBuffers[currentFrame]);

    vkResetCommandBuffer(swapchainCommandBuffers[currentFrame], 0);
    recordSwapchainCommandBuffer(swapchainCommandBuffers[currentFrame], imageIndex);
    logicalDevice->submitGraphicsQueue(currentFrame, &swapchainCommandBuffers[currentFrame]);

    result = logicalDevice->queuePresent(currentFrame, swapChain->getSwapChain(), &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
      framebufferResized = false;
      recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
      throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void VulkanEngine::recreateSwapChain()
  {
    int width = 0, height = 0;
    window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0)
    {
      window->getFramebufferSize(&width, &height);
      glfwWaitEvents();
    }

    logicalDevice->waitIdle();

    framebuffer.reset();
    swapChain.reset();

    physicalDevice->updateSwapChainSupportDetails();

    swapChain = std::make_shared<SwapChain>(physicalDevice, logicalDevice, window);
    framebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, swapChain, commandPool, renderPass,
                                                swapChain->getExtent());

    if (videoExtent.width != 0 && videoExtent.height != 0)
    {
      videoFramebuffer.reset();

      videoFramebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, nullptr, commandPool,
                                                       videoRenderPass, videoExtent);
    }
  }

  void VulkanEngine::createNewFrame() const
  {
    imGuiInstance->createNewFrame();
  }

  void VulkanEngine::renderVideoWidget(const uint32_t imageIndex) const
  {
    const auto widgetName = "Video Output";

    imGuiInstance->dockCenter(widgetName);

    ImGui::Begin(widgetName);

    ImGui::Image(reinterpret_cast<ImTextureID>(videoFramebuffer->getFramebufferImageDescriptorSet(imageIndex)),
                 { static_cast<float>(videoExtent.width), static_cast<float>(videoExtent.height) });

    ImGui::End();
  }

  void VulkanEngine::loadVideoFrameToImage(const int imageIndex) const
  {
    const VkDeviceSize imageSize = videoExtent.width * videoExtent.height * 4; // RGBA format

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Buffers::createBuffer(logicalDevice, physicalDevice, imageSize,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, videoFrameData->data(), imageSize);
    vkUnmapMemory(logicalDevice->getDevice(), stagingBufferMemory);

    Images::transitionImageLayout(logicalDevice, commandPool, videoTextureImages[imageIndex],
                                  VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

    const VkCommandBuffer commandBuffer = Buffers::beginSingleTimeCommands(logicalDevice, commandPool);

    const VkBufferImageCopy region {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1
      },
      .imageOffset = {0, 0, 0},
      .imageExtent = {videoExtent.width, videoExtent.height, 1}
    };

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, videoTextureImages[imageIndex],
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    Buffers::endSingleTimeCommands(logicalDevice, commandPool, logicalDevice->getGraphicsQueue(), commandBuffer);

    Images::transitionImageLayout(logicalDevice, commandPool, videoTextureImages[imageIndex],
                                  VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    vkDestroyBuffer(logicalDevice->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice->getDevice(), stagingBufferMemory, nullptr);
  }

  void VulkanEngine::setupVideoTexture()
  {
    // Create Image
    constexpr size_t numImages = 3;
    videoTextureImageMemory.resize(numImages);
    videoTextureImageViews.resize(numImages);
    videoTextureImages.resize(numImages);
    videoTextureImageInfos.resize(numImages);

    constexpr auto imageFormat = VK_FORMAT_R8G8B8A8_UNORM;

    for (int i = 0; i < numImages; i++)
    {
      Images::createImage(logicalDevice, physicalDevice, videoExtent.width, videoExtent.height, 1,
                          1, VK_SAMPLE_COUNT_1_BIT, imageFormat, VK_IMAGE_TILING_OPTIMAL,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, videoTextureImages[i],
                          videoTextureImageMemory[i], VK_IMAGE_TYPE_2D);

      videoTextureImageViews[i] = Images::createImageView(logicalDevice, videoTextureImages[i],
                                                         imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);

      Images::transitionImageLayout(this->logicalDevice, commandPool, videoTextureImages[i], imageFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
    }

    // Create Sampler
    constexpr VkSamplerCreateInfo samplerInfo {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
      .mipLodBias = 0.0f,
      .anisotropyEnable = VK_FALSE,
      .maxAnisotropy = 1.0f,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE
    };

    if (vkCreateSampler(this->logicalDevice->getDevice(), &samplerInfo, nullptr, &videoTextureSampler) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create image sampler!");
    }

    // Setup Image Info
    for (int i = 0; i < videoTextureImageInfos.capacity(); i++)
    {
      videoTextureImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      videoTextureImageInfos[i].imageView = videoTextureImageViews[i];
      videoTextureImageInfos[i].sampler = videoTextureSampler;
    }
  }

  void VulkanEngine::destroyVideoTexture() const
  {
    vkDestroySampler(logicalDevice->getDevice(), videoTextureSampler, nullptr);

    for (const auto& imageView : videoTextureImageViews)
    {
      vkDestroyImageView(logicalDevice->getDevice(), imageView, nullptr);
    }

    for (const auto& imageMemory : videoTextureImageMemory)
    {
      vkFreeMemory(logicalDevice->getDevice(), imageMemory, nullptr);
    }

    for (const auto& image : videoTextureImages)
    {
      vkDestroyImage(logicalDevice->getDevice(), image, nullptr);
    }
  }
} // VkEngine