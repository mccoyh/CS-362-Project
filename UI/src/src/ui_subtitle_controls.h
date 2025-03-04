#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <imgui.h>

class UISubtitleControls {
public:
    // Subtitle configuration structure
    struct SubtitleConfig {
        bool enabled;
        std::string currentLanguage;
        float fontSize;
        ImVec4 textColor;
        ImVec4 backgroundColor;
    };

    // Subtitle entry for rendering
    struct SubtitleEntry {
        double startTime;
        double endTime;
        std::string text;
        std::string language;
    };

private:
    // Current subtitle configuration
    SubtitleConfig subtitleConfig;

    // Loaded subtitles
    std::vector<SubtitleEntry> subtitles;

    // Supported languages
    static const std::vector<std::string> SUPPORTED_LANGUAGES;

    // Subtitle file parsing
    bool parseSrtSubtitles(const std::filesystem::path& filepath);
    bool parseVttSubtitles(const std::filesystem::path& filepath);
    bool parseAssSubtitles(const std::filesystem::path& filepath);

    // Language detection and translation
    std::string detectLanguage(const std::string& text) const;
    std::string translateSubtitle(const std::string& text, 
                                  const std::string& sourceLanguage, 
                                  const std::string& targetLanguage) const;

    // Rendering helpers
    void renderSubtitleControls();
    void renderSubtitlePreview();

public:
    UISubtitleControls();

    // Main rendering method
    void render();

    // Subtitle file loading
    bool loadSubtitles(const std::filesystem::path& filepath);

    // Subtitle rendering for video playback
    void renderSubtitlesForVideo(double currentVideoTime);

    // Configuration methods
    void toggleSubtitles();
    void setSubtitleLanguage(const std::string& language);
    void adjustFontSize(float size);
    void setTextColor(const ImVec4& color);
    void setBackgroundColor(const ImVec4& color);

    // Query methods
    bool areSubtitlesEnabled() const;
    std::string getCurrentLanguage() const;
    static std::vector<std::string> getSupportedLanguages();
};

// Utility class for subtitle-related operations
class SubtitleUtils {
public:
    // Supported subtitle formats
    static const std::vector<std::string> SUPPORTED_FORMATS;

    // Check if a file is a supported subtitle format
    static bool isSupportedSubtitleFormat(const std::filesystem::path& filepath);

    // Convert subtitle time format (for different subtitle formats)
    static double convertSubtitleTime(const std::string& timeString);
};