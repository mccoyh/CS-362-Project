#include <iostream>
#include <stdexcept>
#include <memory>
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
    VkPhysicalDevice vkPhysicalDevice;
    VkQueue vkGraphicsQueue;
    ImGuiContext* imguiContext;
    UIMainWindow mainWindow;
    bool isRunning;

    void initializeSDL() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }
    }

    void initializeVulkan() {
        // Vulkan instance creation with more robust setup
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = MediaPlayerConfig::APP_NAME;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Enable validation layers in debug mode
        #ifdef _DEBUG
        const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
        #endif

        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance");
        }

        // Select physical device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
        vkPhysicalDevice = devices[0];  // Simple device selection
    }

    void initializeWindow() {
        SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(
            SDL_WINDOW_VULKAN | 
            SDL_WINDOW_RESIZABLE | 
            SDL_WINDOW_HIGH_PIXEL_DENSITY
        );

        window = SDL_CreateWindow(
            MediaPlayerConfig::APP_NAME, 
            1280, 720, 
            windowFlags
        );

        if (!window) {
            throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
        }
    }

    void initializeImGui() {
        IMGUI_CHECKVERSION();
        imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(imguiContext);
        
        // Setup ImGui style
        ImGui::StyleColorsDark();
        
        // Setup Platform/Renderer bindings
        if (!ImGui_ImplSDL3_InitForVulkan(window)) {
            throw std::runtime_error("Failed to initialize ImGui SDL3 backend");
        }
        
        // Additional ImGui configuration
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    void processEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    isRunning = false;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    isRunning = false;
                    break;
            }
        }
    }

    void render() {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        
        // Render main window
        mainWindow.render();
        
        ImGui::Render();
    }

    void cleanup() {
        // Ensure cleanup is idempotent
        if (imguiContext) {
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext(imguiContext);
            imguiContext = nullptr;
        }

        if (vkDevice) {
            vkDestroyDevice(vkDevice, nullptr);
            vkDevice = VK_NULL_HANDLE;
        }

        if (vkInstance) {
            vkDestroyInstance(vkInstance, nullptr);
            vkInstance = VK_NULL_HANDLE;
        }

        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }

public:
    DesktopMediaPlayerApp() : 
        window(nullptr), 
        vkInstance(VK_NULL_HANDLE), 
        vkDevice(VK_NULL_HANDLE), 
        vkPhysicalDevice(VK_NULL_HANDLE),
        vkGraphicsQueue(VK_NULL_HANDLE),
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
            
            // Optional: Add frame timing or limit
            SDL_Delay(16);  // Roughly 60 FPS
        }
    }
};

int main() {
    try {
        DesktopMediaPlayerApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal application error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
