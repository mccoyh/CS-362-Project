#ifndef INSTANCE_H
#define INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <array>

namespace VkEngine {

constexpr std::array<const char*, 1> validationLayers {
  "VK_LAYER_KHRONOS_validation"
};

class Instance {
public:
  Instance();
  ~Instance();

  [[nodiscard]] VkInstance getInstance() const;

private:
  VkInstance instance = VK_NULL_HANDLE;

  static bool checkValidationLayerSupport();

  static std::vector<const char*> getRequiredExtensions();
};

} // VkEngine

#endif //INSTANCE_H
