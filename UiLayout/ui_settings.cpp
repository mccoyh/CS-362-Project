#include "ui_settings.h"
#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

UISettings::UISettings() {
    // Determine config file path
    #ifdef _WIN32
    configFilePath = std::filesystem::path(std::getenv("APPDATA")) / "DesktopMediaPlayer" / "config.json";
    #else
    configFilePath = std::filesystem::path(std::getenv("HOME")) / ".config" / "desktop-media-player" / "config.json";
    #endif

    // Ensure config directory exists
    std::filesystem::create_directories(configFilePath.parent_path());

    // Initialize default settings
    currentSettings = {
        Theme::Dark,           // Theme
        ColorScheme::Default,  // Color Scheme
        true,                  // Hardware Acceleration
        true,                  // Subtitles Auto Load
        "en",                  // Default Subtitle Language
        "",                    // Video Library Path
        0.5f,                 // Playback Volume
        false                  // Mute Sound
    };

    // Try to load existing settings
    loadSettings();
}

UISettings::~UISettings() {
    // Save settings on destruction
    saveSettings();
}

void UISettings::render() {
    ImGui::Begin("Application Settings");

    // Render different settings sections
    renderThemeSettings();
    renderSubtitleSettings();
    renderAudioSettings();
    renderVideoLibrarySettings();

    ImGui::End();
}

void UISettings::renderThemeSettings() {
    ImGui::SeparatorText("Theme Settings");

    // Theme selection
    const char* themeItems[] = { "Dark", "Light", "System" };
    int currentTheme = static_cast<int>(currentSettings.currentTheme);
    ImGui::Text("Theme:");
    ImGui::SameLine();
    if (ImGui::Combo("##Theme", &currentTheme, themeItems, IM_ARRAYSIZE(themeItems))) {
        setTheme(static_cast<Theme>(currentTheme));
    }

    // Color scheme selection
    const char* colorSchemeItems[] = { 
        "Default", "Blue", "Green", "Purple", "Red" 
    };
    int currentColorScheme = static_cast<int>(currentSettings.colorScheme);
    ImGui::Text("Color Scheme:");
    ImGui::SameLine();
    if (ImGui::Combo("##ColorScheme", &currentColorScheme, 
                     colorSchemeItems, IM_ARRAYSIZE(colorSchemeItems))) {
        setColorScheme(static_cast<ColorScheme>(currentColorScheme));
    }
}

void UISettings::renderSubtitleSettings() {
    ImGui::SeparatorText("Subtitle Settings");

    // Auto-load subtitles
    ImGui::Checkbox("Auto-load Subtitles", &currentSettings.subtitlesAutoLoad);

    // Default subtitle language
    ImGui::Text("Default Language:");
    ImGui::SameLine();
    
    const auto supportedLanguages = getSupportedSubtitleLanguages();
    static int currentLanguageIndex = 0;
    
    if (ImGui::BeginCombo("##SubtitleLanguage", 
                          currentSettings.defaultSubtitleLanguage.c_str())) {
        for (int n = 0; n < supportedLanguages.size(); n++) {
            bool isSelected = (currentSettings.defaultSubtitleLanguage == supportedLanguages[n]);
            if (ImGui::Selectable(supportedLanguages[n].c_str(), isSelected)) {
                currentLanguageIndex = n;
                setDefaultSubtitleLanguage(supportedLanguages[n]);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void UISettings::renderAudioSettings() {
    ImGui::SeparatorText("Audio Settings");

    // Hardware acceleration
    ImGui::Checkbox("Hardware Acceleration", &currentSettings.hardwareAcceleration);

    // Volume control
    float volume = currentSettings.playbackVolume;
    ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
    if (volume != currentSettings.playbackVolume) {
        adjustPlaybackVolume(volume);
    }

    // Mute toggle
    ImGui::Checkbox("Mute", &currentSettings.muteSound);
}

void UISettings::renderVideoLibrarySettings() {
    ImGui::SeparatorText("Video Library");

    // Video library path
    std::string libraryPath = currentSettings.videoLibraryPath.string();
    ImGui::Text("Library Path:");
    ImGui::SameLine();
    ImGui::InputText("##LibraryPath", &libraryPath);

    // Browse button
    ImGui::SameLine();
    if (ImGui::Button("Browse")) {
        // TODO: Implement file dialog for selecting library path
        // This would typically use a platform-specific file dialog
        ImGui::OpenPopup("Select Video Library");
    }

    // Update library path if changed
    if (!libraryPath.empty() && 
        libraryPath != currentSettings.videoLibraryPath.string()) {
        setVideoLibraryPath(libraryPath);
    }
}

void UISettings::loadSettings() {
    // Try to load from config file
    if (std::filesystem::exists(configFilePath)) {
        try {
            std::string configData = ConfigurationManager::loadConfiguration(
                configFilePath.string()
            );
            
            // TODO: Parse JSON configuration
            // This is a placeholder and would need actual JSON parsing
        } catch (const std::exception& e) {
            std::cerr << "Failed to load settings: " << e.what() << std::endl;
            
            // Fallback to default settings
            currentSettings = ApplicationSettings{};
        }
    }

    // Validate loaded settings
    validateSettings();
}

void UISettings::saveSettings() {
    try {
        // Generate configuration JSON
        std::string configData = ConfigurationManager::generateDefaultConfig();
        
        ConfigurationManager::saveConfiguration(
            configFilePath.string(), 
            configData
        );
    } catch (const std::exception& e) {
        std::cerr << "Failed to save settings: " << e.what() << std::endl;
    }
}

void UISettings::validateSettings() {
    // Validate and correct settings if needed
    
    // Ensure theme is valid
    if (static_cast<int>(currentSettings.currentTheme) < 0 || 
        static_cast<int>(currentSettings.currentTheme) >= 3) {
        currentSettings.currentTheme = Theme::Dark;
    }

    // Validate color scheme
    if (static_cast<int>(currentSettings.colorScheme) < 0 || 
        static_cast<int>(currentSettings.colorScheme) >= 5) {
        currentSettings.colorScheme = ColorScheme::Default;
    }

    // Validate volume
    currentSettings.playbackVolume = std::clamp(
        currentSettings.playbackVolume, 0.0f, 1.0f
    );

    // Validate video library path
    if (!currentSettings.videoLibraryPath.empty() && 
        !std::filesystem::exists(currentSettings.videoLibraryPath)) {
        currentSettings.videoLibraryPath.clear();
    }
}

void UISettings::setTheme(Theme theme) {
    currentSettings.currentTheme = theme;
    applyTheme();
}

void UISettings::setColorScheme(ColorScheme scheme) {
    currentSettings.colorScheme = scheme;
    applyTheme();
}

void UISettings::applyTheme() {
    // Actual theme application would depend on the UI framework
    ImGuiStyle& style = ImGui::GetStyle();

    switch (currentSettings.colorScheme) {
        case ColorScheme::Blue:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.2f, 1.0f);
            break;
        case ColorScheme::Green:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.2f, 0.1f, 1.0f);
            break;
        case ColorScheme::Purple:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.1f, 0.2f, 1.0f);
            break;
        case ColorScheme::Red:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.1f, 0.1f, 1.0f);
            break;
        default:
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
            break;
    }

    // Apply theme-specific settings
    switch (currentSettings.currentTheme) {
        case Theme::Dark:
            ImGui::StyleColorsDark();
            break;
        case Theme::Light:
            ImGui::StyleColorsLight();
            break;
        case Theme::System:
        default:
            // Use system default or fallback to dark
            ImGui::StyleColorsDark();
            break;
    }
}

void UISettings::toggleHardwareAcceleration() {
    currentSettings.hardwareAcceleration = !currentSettings.hardwareAcceleration;
    // TODO: Actually apply hardware acceleration setting
    std::cout << "Hardware Acceleration: " 
              << (currentSettings.hardwareAcceleration ? "Enabled" : "Disabled") 
              << std::endl;
}

void UISettings::setVideoLibraryPath(const std::filesystem::path& path) {
    // Validate path
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        currentSettings.videoLibraryPath = path;
    } else {
        std::cerr << "Invalid library path: " << path << std::endl;
    }
}

void UISettings::setDefaultSubtitleLanguage(const std::string& language) {
    // Validate language
    const auto supportedLanguages = getSupportedSubtitleLanguages();
    auto it = std::find(
        supportedLanguages.begin(), 
        supportedLanguages.end(), 
        language
    );

    if (it != supportedLanguages.end()) {
        currentSettings.defaultSubtitleLanguage = language;
    } else {
        std::cerr << "Unsupported subtitle language: " << language << std::endl;
    }
}

void UISettings::adjustPlaybackVolume(float volume) {
    currentSettings.playbackVolume = std::clamp(volume, 0.0f, 1.0f);
    // TODO: Actually apply volume setting to system audio
    std::cout << "Playback Volume: " << currentSettings.playbackVolume << std::endl;
}

void UISettings::toggleMute() {
    currentSettings.muteSound = !currentSettings.muteSound;
    // TODO: Actually mute/unmute system audio
    std::cout << "Audio Muted: " 
              << (currentSettings.muteSound ? "Yes" : "No") 
              << std::endl;
}

UISettings::Theme UISettings::getCurrentTheme() const {
    return currentSettings.currentTheme;
}

UISettings::ColorScheme UISettings::getCurrentColorScheme() const {
    return currentSettings.colorScheme;
}

bool UISettings::isHardwareAccelerationEnabled() const {
    return currentSettings.hardwareAcceleration;
}

std::filesystem::path UISettings::getVideoLibraryPath() const {
    return currentSettings.videoLibraryPath;
}

std::vector<std::string> UISettings::getSupportedSubtitleLanguages() {
    return {
        "en", "es", "fr", "de", "it", 
        "ja", "ko", "zh", "ru", "ar", 
        "pt", "nl", "pl", "tr", "hi"
    };
}

std::vector<std::string> UISettings::getAvailableThemes() {
    return {"Dark", "Light", "System"};
}

// Configuration Manager Implementation
bool ConfigurationManager::saveConfiguration(const std::filesystem::path& filepath, 
                                             const std::string& configData) {
    try
        std::ofstream configFile(filepath);
        if (!configFile.is_open()) {
            throw std::runtime_error("Could not open config file for writing");
        }
        // Continuation of Configuration Manager Implementation
bool ConfigurationManager::saveConfiguration(const std::filesystem::path& filepath, 
                                             const std::string& configData) {
    try {
        std::ofstream configFile(filepath);
        if (!configFile.is_open()) {
            throw std::runtime_error("Could not open config file for writing");
        }

        configFile << configData;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving configuration: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigurationManager::loadConfiguration(const std::filesystem::path& filepath) {
    try {
        std::ifstream configFile(filepath);
        if (!configFile.is_open()) {
            throw std::runtime_error("Could not open config file for reading");
        }

        std::stringstream buffer;
        buffer << configFile.rdbuf();
        return buffer.str();
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return generateDefaultConfig();
    }
}

bool ConfigurationManager::validateConfigFile(const std::filesystem::path& filepath) {
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        return false;
    }

    // Check file size (prevent extremely large configs)
    auto fileSize = std::filesystem::file_size(filepath);
    if (fileSize > 1024 * 1024) { // 1 MB max
        return false;
    }

    try {
        std::ifstream configFile(filepath);
        if (!configFile.is_open()) {
            return false;
        }

        // TODO: Add actual JSON validation
        // This would involve parsing the JSON and checking for required fields
        return true;
    } catch (...) {
        return false;
    }
}

std::string ConfigurationManager::generateDefaultConfig() {
    // Generate a basic JSON configuration
    return R"({
    "application": {
        "name": "Desktop Media Player",
        "version": "1.0.0"
    },
    "theme": {
        "mode": "dark",
        "color_scheme": "default"
    },
    "video": {
        "hardware_acceleration": true,
        "library_path": ""
    },
    "subtitles": {
        "auto_load": true,
        "default_language": "en"
    },
    "audio": {
        "volume": 0.5,
        "mute": false
    }
})";
}
