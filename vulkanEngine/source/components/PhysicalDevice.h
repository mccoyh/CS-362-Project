#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <memory>

namespace VkEngine {

class Instance;

const std::vector deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
  // "VK_KHR_portability_subset"
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] bool isComplete() const
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice {
public:
  PhysicalDevice(const std::shared_ptr<Instance>& instance, VkSurfaceKHR& surface);

  [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;

  [[nodiscard]] QueueFamilyIndices getQueueFamilies() const;

  [[nodiscard]] SwapChainSupportDetails getSwapChainSupport() const;

  [[nodiscard]] VkSampleCountFlagBits getMsaaSamples() const;

  [[nodiscard]] uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

  void updateSwapChainSupportDetails();

private:
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  VkSurfaceKHR& surface;

  VkSampleCountFlagBits msaaSamples;

  QueueFamilyIndices queueFamilyIndices;

  SwapChainSupportDetails swapChainSupportDetails;

  void pickPhysicalDevice(const std::shared_ptr<Instance>& instance);

  [[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice device) const;

  [[nodiscard]] QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

  [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

  [[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;

  static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};

} // VkEngine

#endif //PHYSICALDEVICE_H
