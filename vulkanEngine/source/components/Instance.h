#ifndef INSTANCE_H
#define INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <array>

namespace vkEngine {

constexpr std::array<const char*, 1> validationLayers {
  "VK_LAYER_KHRONOS_validation"
};

class Instance {
public:
  Instance();
  ~Instance();

  VkInstance& getInstance();

private:
  VkInstance instance{};

  static bool checkValidationLayerSupport();

  static std::vector<const char*> getRequiredExtensions();
};

} // vkEngine

#endif //INSTANCE_H
