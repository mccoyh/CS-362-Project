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

  void VulkanEngine::loadVideoFrame(std::shared_ptr<std::vector<uint8_t>> frameData, const int width, const int height)
  {
    videoFrameData = std::move(frameData);
    videoWidth = width;
    videoHeight = height;
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

  void VulkanEngine::recordVideoCommandBuffer(const VkCommandBuffer& commandBuffer, const uint32_t imageIndex) const
  {
    recordCommandBuffer(commandBuffer, imageIndex, [this](const VkCommandBuffer& cmdBuffer,
                      const uint32_t imgIndex)
    {
      if (videoExtent.width == 0 || videoExtent.height == 0)
      {
        return;
      }

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