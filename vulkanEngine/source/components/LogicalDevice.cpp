#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Instance.h"
#include <stdexcept>
#include <array>
#include <set>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace VkEngine {
  LogicalDevice::LogicalDevice(const std::shared_ptr<PhysicalDevice>& physicalDevice)
    : device(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE)
  {
    createDevice(physicalDevice);

    createSyncObjects();
  }

  LogicalDevice::~LogicalDevice()
  {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
      vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyDevice(device, nullptr);
  }

  VkDevice& LogicalDevice::getDevice()
  {
    return device;
  }

  void LogicalDevice::waitIdle() const
  {
    vkDeviceWaitIdle(device);
  }

  VkQueue& LogicalDevice::getGraphicsQueue()
  {
    return graphicsQueue;
  }

  VkQueue& LogicalDevice::getPresentQueue()
  {
    return presentQueue;
  }

  void LogicalDevice::submitGraphicsQueue(uint32_t currentFrame, const VkCommandBuffer *commandBuffer) const
  {
    const std::array waitSemaphores = {
      imageAvailableSemaphores[currentFrame]
    };
    constexpr VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    const VkSubmitInfo submitInfo {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
      .pWaitSemaphores = waitSemaphores.data(),
      .pWaitDstStageMask = waitStages,
      .commandBufferCount = 1,
      .pCommandBuffers = commandBuffer,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &renderFinishedSemaphores[currentFrame]
    };

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to submit draw command buffer!");
    }
  }

  void LogicalDevice::waitForGraphicsFences(const uint32_t currentFrame) const
  {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
  }

  void LogicalDevice::resetGraphicsFences(const uint32_t currentFrame) const
  {
    vkResetFences(device, 1, &inFlightFences[currentFrame]);
  }

  VkResult LogicalDevice::queuePresent(uint32_t currentFrame, const VkSwapchainKHR& swapchain,
                                       const uint32_t* imageIndex) const
  {
    const std::array<VkSemaphore, 1> waitSemaphores = {
      renderFinishedSemaphores[currentFrame]
    };

    const VkPresentInfoKHR presentInfo {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
      .pWaitSemaphores = waitSemaphores.data(),
      .swapchainCount = 1,
      .pSwapchains = &swapchain,
      .pImageIndices = imageIndex,
      .pResults = nullptr
    };

    return vkQueuePresentKHR(presentQueue, &presentInfo);
  }

  VkResult LogicalDevice::acquireNextImage(uint32_t currentFrame, const VkSwapchainKHR& swapchain,
                                           uint32_t* imageIndex) const
  {
    return vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
                                 VK_NULL_HANDLE, imageIndex);
  }

  void LogicalDevice::createDevice(const std::shared_ptr<PhysicalDevice> &physicalDevice)
  {
    auto [graphicsFamily, presentFamily] = physicalDevice->getQueueFamilies();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilies = {graphicsFamily.value(), presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
      const VkDeviceQueueCreateInfo queueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
      };

      queueCreateInfos.push_back(queueCreateInfo);
    }

    constexpr VkPhysicalDeviceFeatures deviceFeatures {
      .samplerAnisotropy = VK_TRUE
    };

    const VkDeviceCreateInfo createInfo {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0,
      .ppEnabledLayerNames = enableValidationLayers ? validationLayers.data() : nullptr,
      .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
      .ppEnabledExtensionNames = deviceExtensions.data(),
      .pEnabledFeatures = &deviceFeatures
    };

    if (vkCreateDevice(physicalDevice->getPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily.value(), 0, &presentQueue);
  }

  void LogicalDevice::createSyncObjects()
  {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    constexpr VkSemaphoreCreateInfo semaphoreInfo {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    constexpr VkFenceCreateInfo fenceInfo {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
          vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
          vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create graphics sync objects!");
      }
    }
  }
} // VkEngine