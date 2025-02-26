#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <memory>

namespace VkEngine {

class Instance;

class Window {
public:
  Window(int width, int height, const char* title, const std::shared_ptr<Instance>& instance, bool fullscreen);
  ~Window();

  [[nodiscard]] bool isOpen() const;

  void update();

  void getFramebufferSize(int* width, int* height) const;

  VkSurfaceKHR& getSurface();

  [[nodiscard]] bool keyIsPressed(int key) const;

  [[nodiscard]] bool buttonDown(int button) const;

  void getCursorPos(double& xpos, double& ypos) const;

  void getPreviousCursorPos(double& xpos, double& ypos) const;

  void initImGui() const;

  [[nodiscard]] double getScroll() const;

  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
  GLFWwindow* window;

  std::shared_ptr<Instance> instance;
  VkSurfaceKHR surface = VK_NULL_HANDLE;

  double previousMouseX;
  double previousMouseY;
  double mouseX;
  double mouseY;

  double scroll;

  std::unordered_map<int, bool> keysPressed;

  void createSurface();

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

} // VkEngine

#endif //WINDOW_H
