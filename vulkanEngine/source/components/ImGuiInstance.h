#ifndef IMGUIINSTANCE_H
#define IMGUIINSTANCE_H

#include <memory>
#include <vulkan/vulkan.h>
#include <imgui.h>

namespace VkEngine {

class Window;
class Instance;
class PhysicalDevice;
class LogicalDevice;
class RenderPass;
class GuiPipeline;

class ImGuiInstance {
public:
  ImGuiInstance(const VkCommandPool& commandPool, const std::shared_ptr<Window>& window,
                const std::shared_ptr<Instance>& instance, const std::shared_ptr<PhysicalDevice>& physicalDevice,
                const std::shared_ptr<LogicalDevice>& logicalDevice, const std::shared_ptr<RenderPass>& renderPass,
                const std::shared_ptr<GuiPipeline>& guiPipeline, bool useDockSpace);
  ~ImGuiInstance();

  void createNewFrame();

  void dockTop(const char* widget) const;

  void dockBottom(const char* widget) const;

  void dockLeft(const char* widget) const;

  void dockRight(const char* widget) const;

  void dockCenter(const char* widget) const;

  void setTopDockPercent(float percent);

  void setBottomDockPercent(float percent);

  void setLeftDockPercent(float percent);

  void setRightDockPercent(float percent);

private:
  bool dockNeedsUpdate = true;

  bool useDockSpace;

  float topDockPercent = 0.15f;
  float bottomDockPercent = 0.2f;
  float leftDockPercent = 0.3f;
  float rightDockPercent = 0.3f;

  ImGuiID mainDock = 0;
  ImGuiID topDock = 0;
  ImGuiID bottomDock = 0;
  ImGuiID leftDock = 0;
  ImGuiID rightDock = 0;
  ImGuiID centerDock = 0;

  static void setupStyles();
};

} // VkEngine

#endif //IMGUIINSTANCE_H
