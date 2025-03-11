#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace VkEngine {

class PhysicalDevice;
class LogicalDevice;
class Window;

class SwapChain {
public:
  SwapChain(const std::shared_ptr<PhysicalDevice>& physicalDevice, const std::shared_ptr<LogicalDevice>& logicalDevice,
            const std::shared_ptr<Window>& window);
  ~SwapChain();

  [[nodiscard]] VkFormat getImageFormat() const;
  [[nodiscard]] VkExtent2D getExtent() const;
  [[nodiscard]] VkSwapchainKHR getSwapChain() const;

  [[nodiscard]] std::vector<VkImageView>& getImageViews();

private:
  std::shared_ptr<PhysicalDevice> physicalDevice;
  std::shared_ptr<LogicalDevice> logicalDevice;
  std::shared_ptr<Window> window;

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat{};
  VkExtent2D swapChainExtent{};
  std::vector<VkImageView> swapChainImageViews;

  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

  static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

  [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

  static uint32_t chooseSwapImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

  void createSwapChain();

  void createImageViews();
};

} // VkEngine

#endif //SWAPCHAIN_H
