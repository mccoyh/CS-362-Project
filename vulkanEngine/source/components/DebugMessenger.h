#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H

#include <vulkan/vulkan.h>
#include <memory>

namespace VkEngine {

class Instance;

class DebugMessenger {
public:
  explicit DebugMessenger(const std::shared_ptr<Instance>& instance);
  ~DebugMessenger();

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData);

  static void populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  static const char* readMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity);

private:
  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  std::shared_ptr<Instance> instance;
};

} // VkEngine

#endif //DEBUGMESSENGER_H
