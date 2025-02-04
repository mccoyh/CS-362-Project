#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H

#include <vulkan/vulkan.h>

namespace VkEngine {

class DebugMessenger {
public:
  explicit DebugMessenger(VkInstance& instance);
  ~DebugMessenger();

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData);

  static void populateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  static const char* readMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity);

private:
  VkDebugUtilsMessengerEXT debugMessenger{};
  VkInstance& instance;
};

} // VkEngine

#endif //DEBUGMESSENGER_H
