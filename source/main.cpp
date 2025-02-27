#include "ui_main_window.h"
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
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

        UIMainWindow mainWindow;

        while (vulkanEngine.isActive())
        {
            gui->dockTop("Video Player");
            gui->dockCenter("APP_NAME");

            gui->dockBottom("File Explorer");
            gui->dockBottom("Playlist");
            gui->dockBottom("Application Settings");
            gui->dockBottom("Subtitle Controls");
            gui->dockBottom("Subtitles");

            gui->setBottomDockPercent(0.2);

            mainWindow.render();

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