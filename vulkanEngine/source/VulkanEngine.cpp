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
#include <stdexcept>

#include "utilities/Buffers.h"
#include "utilities/Images.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}


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
  }

  VulkanEngine::~VulkanEngine()
  {
    logicalDevice->waitIdle();

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

    videoFramebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, nullptr, commandPool,
                                                     renderPass, swapChain->getExtent());
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
  ////////////////////////////////////

bool extractFrame(const char* filename, int time, uint8_t** outData, int& outWidth, int& outHeight)
{
    AVFormatContext* fmtCtx = nullptr;
    AVCodecContext* codecCtx = nullptr;
    AVFrame* frame = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();

    if (!frame || !packet) return false;

    avformat_open_input(&fmtCtx, filename, nullptr, nullptr);
    avformat_find_stream_info(fmtCtx, nullptr);

    // Find video stream
    int videoStream = -1;
    for (unsigned int i = 0; i < fmtCtx->nb_streams; i++) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) return false;

    AVCodecParameters* codecParams = fmtCtx->streams[videoStream]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
    codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, codecParams);
    avcodec_open2(codecCtx, codec, nullptr);

    // Seek to frame
    int64_t targetPts = time * fmtCtx->streams[videoStream]->time_base.den /
                        fmtCtx->streams[videoStream]->time_base.num;

    av_seek_frame(fmtCtx, videoStream, targetPts, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(codecCtx);

    // Decode frame
    while (av_read_frame(fmtCtx, packet) >= 0) {
        if (packet->stream_index == videoStream) {
            avcodec_send_packet(codecCtx, packet);
            if (avcodec_receive_frame(codecCtx, frame) == 0) {
                // Convert to RGBA
                SwsContext* swsCtx = sws_getContext(frame->width, frame->height, codecCtx->pix_fmt,
                                                     frame->width, frame->height, AV_PIX_FMT_RGBA,
                                                     SWS_BILINEAR, nullptr, nullptr, nullptr);

                *outData = new uint8_t[frame->width * frame->height * 4];
                uint8_t* dst[1] = { *outData };
                int dstStride[1] = { frame->width * 4 };

                sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, dst, dstStride);

                outWidth = frame->width;
                outHeight = frame->height;

                sws_freeContext(swsCtx);
                break;
            }
        }
        av_packet_unref(packet);
    }

    // Cleanup
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&fmtCtx);
    return *outData != nullptr;
}

void loadFrameToImage(const VkCommandPool& commandPool, const char* videoPath, int frameIndex, int framebufferIndex,
                      const std::shared_ptr<LogicalDevice>& logicalDevice,
                      const std::shared_ptr<PhysicalDevice>& physicalDevice,
                      std::vector<VkImage>& framebufferImages,
                      std::vector<VkDeviceMemory>& framebufferImageMemory, double frameRate, VkExtent2D clipExtent)
{
    int frameWidth, frameHeight;
    uint8_t* frameData = nullptr;

    // Calculate time for the frame based on frameIndex and frameRate
    int timeInMilliseconds = static_cast<int>(frameIndex * 1000.0 / frameRate); // Convert to time in milliseconds

    // Load frame from FFmpeg based on calculated time
    if (!extractFrame(videoPath, timeInMilliseconds, &frameData, frameWidth, frameHeight))
    {
        throw std::runtime_error("Failed to load video frame!");
    }

    const VkDeviceSize imageSize = frameWidth * frameHeight * 4; // RGBA format

    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Buffers::createBuffer(logicalDevice, physicalDevice, imageSize,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          stagingBuffer, stagingBufferMemory);

    // Copy frame data into staging buffer
    void* data;
    vkMapMemory(logicalDevice->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, frameData, imageSize);
    vkUnmapMemory(logicalDevice->getDevice(), stagingBufferMemory);

    delete[] frameData; // Free FFmpeg frame buffer

    // Get image dimensions
    VkExtent3D imageExtent;
    imageExtent.width = std::min(frameWidth, static_cast<int>(clipExtent.width));
    imageExtent.height = std::min(frameHeight, static_cast<int>(clipExtent.height));;
    imageExtent.depth = 1;

    // Ensure framebuffer image is correctly sized
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice->getDevice(), framebufferImages[framebufferIndex], &memRequirements);

    if (imageExtent.width > memRequirements.size || imageExtent.height > memRequirements.size) {
        throw std::runtime_error("Frame size exceeds allocated Vulkan image size!");
    }



    // Transition framebuffer image to TRANSFER_DST_OPTIMAL before copying
    Images::transitionImageLayout(logicalDevice, commandPool, framebufferImages[framebufferIndex],
                                  VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

    // Create a command buffer for the copy operation
    VkCommandBuffer commandBuffer = Buffers::beginSingleTimeCommands(logicalDevice, commandPool);

    // Copy buffer to Vulkan image
    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = imageExtent;

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, framebufferImages[framebufferIndex],
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // End the single-time command buffer
    Buffers::endSingleTimeCommands(logicalDevice, commandPool, logicalDevice->getGraphicsQueue(), commandBuffer);

    // Transition image to SHADER_READ_ONLY_OPTIMAL for rendering
    Images::transitionImageLayout(logicalDevice, commandPool, framebufferImages[framebufferIndex],
                                  VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    // Clean up staging buffer
    vkDestroyBuffer(logicalDevice->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice->getDevice(), stagingBufferMemory, nullptr);
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

    loadFrameToImage(commandPool, "assets/sample.mp4", 0, imgIndex, logicalDevice, physicalDevice,
                     videoFramebuffer->framebufferImages, videoFramebuffer->framebufferImageMemory, 30, videoExtent);
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
                                                       renderPass, videoExtent);
    }
  }

  void VulkanEngine::createNewFrame() const
  {
    imGuiInstance->createNewFrame();
  }

  void VulkanEngine::renderVideoWidget(const uint32_t imageIndex)
  {
    const auto widgetName = "Video Output";

    imGuiInstance->dockCenter(widgetName);

    ImGui::Begin(widgetName);

    const auto contentRegionAvailable = ImGui::GetContentRegionAvail();

    const VkExtent2D currentOffscreenViewportExtent {
      .width = static_cast<uint32_t>(std::max(0.0f, contentRegionAvailable.x)),
      .height = static_cast<uint32_t>(std::max(0.0f, contentRegionAvailable.y))
    };

    if (currentOffscreenViewportExtent.width == 0 || currentOffscreenViewportExtent.height == 0)
    {
      videoExtent = currentOffscreenViewportExtent;
      ImGui::End();
      return;
    }

    if (videoExtent.width != currentOffscreenViewportExtent.width ||
        videoExtent.height != currentOffscreenViewportExtent.height)
    {
      videoExtent = currentOffscreenViewportExtent;

      logicalDevice->waitIdle();
      videoFramebuffer.reset();
      videoFramebuffer = std::make_shared<Framebuffer>(physicalDevice, logicalDevice, nullptr, commandPool,
                                                       renderPass, videoExtent);
    }

    ImGui::Image(reinterpret_cast<ImTextureID>(videoFramebuffer->getFramebufferImageDescriptorSet(imageIndex)),
                contentRegionAvailable);

    ImGui::End();
  }
} // VkEngine