#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace VkEngine {

const std::vector deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice {
public:
  PhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);

  [[nodiscard]] VkPhysicalDevice& getPhysicalDevice();

  [[nodiscard]] QueueFamilyIndices& getQueueFamilies();

  [[nodiscard]] SwapChainSupportDetails& getSwapChainSupport();

  [[nodiscard]] VkSampleCountFlagBits getMsaaSamples() const;

private:
  VkPhysicalDevice physicalDevice;

  VkInstance& instance;
  VkSurfaceKHR& surface;

  VkSampleCountFlagBits msaaSamples;

  QueueFamilyIndices queueFamilyIndices;
  SwapChainSupportDetails swapChainSupportDetails;

  void pickPhysicalDevice();

  [[nodiscard]] bool isDeviceSuitable(VkPhysicalDevice device) const;

  [[nodiscard]] QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

  [[nodiscard]] SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

  [[nodiscard]] VkSampleCountFlagBits getMaxUsableSampleCount() const;

  static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};

} // VkEngine

#endif //PHYSICALDEVICE_H
