#include "ui_subtitle_controls.h"
#include "config.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>
#include <algorithm>

// Supported languages
const std::vector<std::string> UISubtitleControls::SUPPORTED_LANGUAGES = {
    "en", "es", "fr", "de", "it", "ja", "ko", "zh", "ru", "ar", 
    "pt", "nl", "pl", "tr", "hi"
};

// Supported subtitle formats
const std::vector<std::string> SubtitleUtils::SUPPORTED_FORMATS = {
    ".srt", ".vtt", ".ass", ".sub", ".idx"
};

UISubtitleControls::UISubtitleControls() {
    // Initialize default subtitle configuration
    subtitleConfig = {
        true,           // enabled
        "en",           // current language
        16.0f,          // font size
        ImVec4(1,1,1,1),// white text
        ImVec4(0,0,0,0.5f) // semi-transparent black background
    };
}

void UISubtitleControls::render() {
    ImGui::Begin("Subtitle Controls");

    // Subtitle controls
    renderSubtitleControls();

    // Subtitle preview
    renderSubtitlePreview();

    ImGui::End();
}

void UISubtitleControls::renderSubtitleControls() {
    // Subtitle toggle
    ImGui::Checkbox("Enable Subtitles", &subtitleConfig.enabled);

    // Language selection
    ImGui::Text("Subtitle Language:");
    if (ImGui::BeginCombo("##LanguageSelect", subtitleConfig.currentLanguage.c_str())) {
        for (const auto& lang : SUPPORTED_LANGUAGES) {
            bool isSelected = (subtitleConfig.currentLanguage == lang);
            if (ImGui::Selectable(lang.c_str(), isSelected)) {
                setSubtitleLanguage(lang);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Font size adjustment
    ImGui::SliderFloat("Font Size", &subtitleConfig.fontSize, 8.0f, 32.0f);

    // Color pickers
    ImGui::ColorEdit4("Text Color", (float*)&subtitleConfig.textColor);
    ImGui::ColorEdit4("Background Color", (float*)&subtitleConfig.backgroundColor);
}

void UISubtitleControls::renderSubtitlePreview() {
    // Display a sample subtitle preview
    if (!subtitles.empty()) {
        ImGui::Separator();
        ImGui::Text("Subtitle Preview:");
        
        // Use the first subtitle as a preview
        ImGui::PushStyleColor(ImGuiCol_Text, subtitleConfig.textColor);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, subtitleConfig.backgroundColor);
        
        ImGui::BeginChild("SubtitlePreview", 
                          ImVec2(ImGui::GetContentRegionAvail().x, 100), 
                          true);
        
        ImGui::SetWindowFontScale(subtitleConfig.fontSize / 16.0f);
        ImGui::TextWrapped("%s", subtitles.front().text.c_str());
        
        ImGui::EndChild();
        
        ImGui::PopStyleColor(2);
    }
}

bool UISubtitleControls::loadSubtitles(const std::filesystem::path& filepath) {
    // Clear existing subtitles
    subtitles.clear();

    // Validate subtitle file
    if (!SubtitleUtils::isSupportedSubtitleFormat(filepath)) {
        std::cerr << "Unsupported subtitle format: " << filepath << std::endl;
        return false;
    }

    // Parse based on file extension
    std::string ext = filepath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    bool loadSuccess = false;
    if (ext == ".srt") {
        loadSuccess = parseSrtSubtitles(filepath);
    } else if (ext == ".vtt") {
        loadSuccess = parseVttSubtitles(filepath);
    } else if (ext == ".ass") {
        loadSuccess = parseAssSubtitles(filepath);
    }

    if (loadSuccess) {
        // Detect subtitle language
        if (!subtitles.empty()) {
            subtitleConfig.currentLanguage = detectLanguage(subtitles.front().text);
        }
    }

    return loadSuccess;
}

bool UISubtitleControls::parseSrtSubtitles(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open subtitle file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    SubtitleEntry currentSubtitle;
    int state = 0; // 0: looking for index, 1: looking for time, 2: reading text

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) {
            // Empty line, reset state
            if (!currentSubtitle.text.empty()) {
                subtitles.push_back(currentSubtitle);
                currentSubtitle = SubtitleEntry();
            }
            state = 0;
            continue;
        }

        switch (state) {
            case 0: // Subtitle index
                if (std::regex_match(line, std::regex("\\d+"))) {
                    state = 1;
                }
                break;

            case 1: // Time codes
                {
                    std::regex timeRegex("(\\d{2}:\\d{2}:\\d{2},\\d{3}) --> (\\d{2}:\\d{2}:\\d{2},\\d{3})");
                    std::smatch matches;
                    if (std::regex_search(line, matches, timeRegex)) {
                        currentSubtitle.startTime = SubtitleUtils::convertSubtitleTime(matches[1]);
                        currentSubtitle.endTime = SubtitleUtils::convertSubtitleTime(matches[2]);
                        state = 2;
                    }
                }
                break;

            case 2: // Subtitle text
                if (!line.empty()) {
                    if (!currentSubtitle.text.empty()) {
                        currentSubtitle.text += " ";
                    }
                    currentSubtitle.text += line;
                }
                break;
        }
    }

    // Add last subtitle if exists
    if (!currentSubtitle.text.empty()) {
        subtitles.push_back(currentSubtitle);
    }

    return !subtitles.empty();
}

bool UISubtitleControls::parseVttSubtitles(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open subtitle file: " << filepath << std::endl;
        return false;
    }

    // Skip WebVTT header
    std::string line;
    std::getline(file, line);
    if (line != "WEBVTT") {
        std::cerr << "Invalid WebVTT file" << std::endl;
        return false;
    }

    SubtitleEntry currentSubtitle;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        // Time code regex
        std::regex timeRegex("(\\d{2}:\\d{2}:\\d{2}\\.\\d{3}) --> (\\d{2}:\\d{2}:\\d{2}\\.\\d{3})");
        std::smatch matches;
        if (std::regex_search(line, matches, timeRegex)) {
            // Save previous subtitle if exists
            if (!currentSubtitle.text.empty()) {
                subtitles.push_back(currentSubtitle);
                currentSubtitle = SubtitleEntry();
            }

            // Parse time codes
            currentSubtitle.startTime = SubtitleUtils::convertSubtitleTime(matches[1]);
            currentSubtitle.endTime = SubtitleUtils::convertSubtitleTime(matches[2]);
        } else if (!line.empty() && currentSubtitle.startTime >= 0) {
            // Subtitle text
            if (!currentSubtitle.text.empty()) {
                currentSubtitle.text += " ";
            }
            currentSubtitle.text += line;
        }
    }

    // Add last subtitle
    if (!currentSubtitle.text.empty()) {
        subtitles.push_back(currentSubtitle);
    }

    return !subtitles.empty();
}

bool UISubtitleControls::parseAssSubtitles(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open subtitle file: " << filepath << std::endl;
        return false;
    }

    // TODO: Implement Advanced SubStation Alpha (ASS) subtitle parsing
    std::cerr << "ASS subtitle parsing not yet implemented" << std::endl;
    return false;
}

void UISubtitleControls::renderSubtitlesForVideo(double currentVideoTime) {
    if (!subtitleConfig.enabled || subtitles.empty()) return;

    // Find current subtitle
    for (const auto& subtitle : subtitles) {
        if (currentVideoTime >= subtitle.startTime && 
            currentVideoTime <= subtitle.endTime) {
            // Render subtitle
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x / 2, 
                       ImGui::GetIO().DisplaySize.y * 0.8f), 
                ImGuiCond_Always, 
                ImVec2(0.5f, 0.5f)
            );
            ImGui::Begin("Subtitles", nullptr, 
                ImGuiWindowFlags_NoBackground | 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoInputs
            );
            
            // Apply subtitle styling
            ImGui::PushStyleColor(ImGuiCol_Text, subtitleConfig.textColor);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, subtitleConfig.backgroundColor);
            ImGui::SetWindowFontScale(subtitleConfig.fontSize / 16.0f);
            
            ImGui::TextWrapped("%s", subtitle.text.c_str());
            
            ImGui::PopStyleColor(2);
            ImGui::End();
            break;
        }
    }
}

std::string UISubtitleControls::detectLanguage(const std::string& text) const {
    // Very basic language detection using character ranges
    bool hasArabic = std::any_of(text.begin(), text.end(), 
        [](char c) { return (c >= 0x0600 && c <= 0x06FF); });
    
    bool hasChinese = std::any_of(text.begin(), text.end(), 
        [](char c) { return (c >= 0x4E00 && c <= 0x9FFF); });
    
    bool hasJapanese = std::any_of(text.begin(), text.end(), 
        [](char c) { 
            return (c >= 0x3040 && c <= 0x309F) ||  // Hiragana
                   (c >= 0x30A0 && c <= 0x30FF);    // Katakana
        });
    
    bool hasKorean = std::any_of(text.begin(), text.end(), 
        [](char c) { return (c >= 0xAC00 && c <= 0xD7A3); });

    bool hasRussian = std::any_of(text.begin(), text.end(), 
        [](char c) { return (c >= 0x0400 && c <= 0x04FF); });

    if (hasArabic) return "ar";
    if (hasChinese) return "zh";
    if (hasJapanese) return "ja";
    if (hasKorean) return "ko";
    if (hasRussian) return "ru";

    // Default to English if no specific language detected
    return "en";
}

std::string UISubtitleControls::translateSubtitle(const std::string& text, 
                                                 const std::string& sourceLanguage, 
                                                 const std::string& targetLanguage) const {
    // Placeholder translation logic
    // In a real implementation, this would use a translation API or library
    if (sourceLanguage == targetLanguage) {
        return text;
    }

    // Very basic "translation" simulation
    std::string translatedText = "[" + targetLanguage + "] " + text;
    return translatedText;
}

void UISubtitleControls::toggleSubtitles() {
    subtitleConfig.enabled = !subtitleConfig.enabled;
}

void UISubtitleControls::setSubtitleLanguage(const std::string& language) {
    // Validate language
    auto it = std::find(SUPPORTED_LANGUAGES.begin(), 
                        SUPPORTED_LANGUAGES.end(), 
                        language);
    
    if (it != SUPPORTED_LANGUAGES.end()) {
        // Translate existing subtitles if they exist
        if (!subtitles.empty()) {
            for (auto& subtitle : subtitles) {
                subtitle.text = translateSubtitle(
                    subtitle.text, 
                    subtitleConfig.currentLanguage, 
                    language
                );
                subtitle.language = language;
            }
        }

        subtitleConfig.currentLanguage = language;
    } else {
        std::cerr << "Unsupported language: " << language << std::endl;
    }
}

void UISubtitleControls::adjustFontSize(float size) {
    subtitleConfig.fontSize = std::clamp(size, 8.0f, 32.0f);
}

void UISubtitleControls::setTextColor(const ImVec4& color) {
    subtitleConfig.textColor = color;
}

void UISubtitleControls::setBackgroundColor(const ImVec4& color) {
    subtitleConfig.backgroundColor = color;
}

bool UISubtitleControls::areSubtitlesEnabled() const {
    return subtitleConfig.enabled;
}

std::string UISubtitleControls::getCurrentLanguage() const {
    return subtitleConfig.currentLanguage;
}

std::vector<std::string> UISubtitleControls::getSupportedLanguages() {
    return SUPPORTED_LANGUAGES;
}

// Subtitle Utilities Implementation
bool SubtitleUtils::isSupportedSubtitleFormat(const std::filesystem::path& filepath) {
    std::string ext = filepath.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(SUPPORTED_FORMATS.begin(), 
                     SUPPORTED_FORMATS.end(), 
                     ext) != SUPPORTED_FORMATS.end();
}

double SubtitleUtils::convertSubtitleTime(const std::string& timeString) {
    // Handle both SRT (00:01:23,456) and VTT (00:01:23.456) formats
    std::regex timeRegex("(\\d{2}):(\\d{2}):(\\d{2})[,.]?(\\d{3})?");
    std::smatch matches;
    
    if (std::regex_match(timeString, matches, timeRegex)) {
        int hours = std::stoi(matches[1]);
        int minutes = std::stoi(matches[2]);
        int seconds = std::stoi(matches[3]);
        int milliseconds = matches[4].matched ? std::stoi(matches[4]) : 0;

        return hours * 3600.0 + minutes * 60.0 + seconds + milliseconds / 1000.0;
    }

    return -1.0; // Invalid time format
}
