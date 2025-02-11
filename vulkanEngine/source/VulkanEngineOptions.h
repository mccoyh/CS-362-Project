#ifndef VULKANENGINEOPTIONS_H
#define VULKANENGINEOPTIONS_H

#include <cstdint>

namespace VkEngine {

struct VulkanEngineOptions {
  bool FULLSCREEN = false;

  uint32_t WINDOW_WIDTH = 600;
  uint32_t WINDOW_HEIGHT = 400;

  const char* WINDOW_TITLE = "Window";
};

} // VkEngine

#endif //VULKANENGINEOPTIONS_H
