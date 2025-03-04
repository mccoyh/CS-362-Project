#include <iostream>
#include <stdexcept>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

#include "config.h"
#include "ui_main_window.h"

class DesktopMediaPlayerApp {
private:
    SDL_Window* window;
    VkInstance vkInstance;
    VkDevice vkDevice;
    ImGuiContext* imguiContext;
    UIMainWindow mainWindow;
    bool isRunning;

    void initializeSDL() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            throw std::runtime_error("Failed to initialize SDL");
        }
    }

    void initializeVulkan() {
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        
        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    void initializeWindow() {
        window = SDL_CreateWindow(
            MediaPlayerConfig::APP_NAME, 
            1280, 720, 
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
        );

        if (!window) {
            throw std::runtime_error("Failed to create window");
        }
    }

    void initializeImGui() {
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(imguiContext);

        // Setup Platform/Renderer bindings
        ImGui_ImplSDL3_InitForVulkan(window);
        ImGui_ImplVulkan_Init(&imguiContext);
    }

    void processEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }
    }

    void render() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        mainWindow.render();

        ImGui::Render();
    }

    void cleanup() {
        // Cleanup ImGui
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(imguiContext);

        // Cleanup Vulkan
        vkDestroyDevice(vkDevice, nullptr);
        vkDestroyInstance(vkInstance, nullptr);

        // Cleanup SDL
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

public:
    DesktopMediaPlayerApp() : 
        window(nullptr), 
        vkInstance(VK_NULL_HANDLE), 
        vkDevice(VK_NULL_HANDLE), 
        imguiContext(nullptr),
        isRunning(true) {
        
        try {
            initializeSDL();
            initializeWindow();
            initializeVulkan();
            initializeImGui();
        } catch (const std::exception& e) {
            std::cerr << "Initialization error: " << e.what() << std::endl;
            cleanup();
            throw;
        }
    }

    ~DesktopMediaPlayerApp() {
        cleanup();
    }

    void run() {
        while (isRunning) {
            processEvents();
            render();
        }
    }
};

int main() {
    try {
        DesktopMediaPlayerApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}