#ifndef VULKANENGINE_H
#define VULKANENGINE_H

#include <memory>

namespace VkEngine {

class Instance;
class Window;

class VulkanEngine {
public:
  VulkanEngine();
  ~VulkanEngine();

  [[nodiscard]] bool isActive() const;

  void render();

private:
  std::unique_ptr<Instance> instance;
  std::shared_ptr<Window> window;
};

} // VkEngine

#endif //VULKANENGINE_H
