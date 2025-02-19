#include <source/VulkanEngine.h>
#include <source/components/ImGuiInstance.h>
#include <imgui.h>
#include <iostream>

int main()
{
  try
  {
    constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 600,
      .WINDOW_HEIGHT = 400,
      .WINDOW_TITLE = "Widget Test"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());
    const auto gui = vulkanEngine.getImGuiInstance();

    while (vulkanEngine.isActive())
    {
      gui->dockBottom("Test Widget");
      
      gui->setBottomDockPercent(0.2);

      ImGui::Begin("Test Widget");

      ImGui::Text("This is a Test Widget");

      ImGui::End();

      vulkanEngine.render();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
