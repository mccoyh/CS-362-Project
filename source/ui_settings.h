#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <imgui.h>

class UISettings {
public:
    // Enum for available themes
    enum class Theme {
        Dark,
        Light,
        System
    };

    // Enum for color schemes
    enum class ColorScheme {
        Default,
        Blue,
        Green,
        Purple,
        Red
    };

private:
    // Application settings structure
    struct ApplicationSettings {
        Theme currentTheme;
        ColorScheme colorScheme;
        bool hardwareAcceleration;
        bool subtitlesAutoLoad;
        std::string defaultSubtitleLanguage;
        std::filesystem::path videoLibraryPath;
        float playbackVolume;
        bool muteSound;
    };

    // Current application settings
    ApplicationSettings currentSettings;

    // Paths for configuration and library
    std::filesystem::path configFilePath;

    // Helper methods
    void loadSettings();
    void saveSettings();
    void applyTheme();
    void validateSettings();

    // Theme-related methods
    void setTheme(Theme theme);
    void setColorScheme(ColorScheme scheme);

    // Rendering helper methods
    void renderThemeSettings();
    void renderSubtitleSettings();
    void renderAudioSettings();
    void renderVideoLibrarySettings();

public:
    UISettings();
    ~UISettings();

    // Main rendering method
    void render();

    // Settings modification methods
    void toggleHardwareAcceleration();
    void setVideoLibraryPath(const std::filesystem::path& path);
    void setDefaultSubtitleLanguage(const std::string& language);
    void adjustPlaybackVolume(float volume);
    void toggleMute();

    // Accessor methods
    Theme getCurrentTheme() const;
    ColorScheme getCurrentColorScheme() const;
    bool isHardwareAccelerationEnabled() const;
    std::filesystem::path getVideoLibraryPath() const;

    // Static utility methods
    static std::vector<std::string> getSupportedSubtitleLanguages();
    static std::vector<std::string> getAvailableThemes();
};

// Configuration file handler
class ConfigurationManager {
public:
    // Save configuration to file
    static bool saveConfiguration(const std::filesystem::path& filepath, 
                                  const std::string& configData);

    // Load configuration from file
    static std::string loadConfiguration(const std::filesystem::path& filepath);

    // Validate configuration file
    static bool validateConfigFile(const std::filesystem::path& filepath);

    // Generate default configuration
    static std::string generateDefaultConfig();
};