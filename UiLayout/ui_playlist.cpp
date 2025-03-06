#include "ui_playlist.h"
#include "config.h"
#include <algorithm>
#include <fstream>
#include <random>
#include <iostream>

extern "C" {
    #include <libavformat/avformat.h>
}

UIPlaylist::UIPlaylist() : 
    currentItemIndex(-1), 
    shuffleMode(false), 
    repeatMode(false) {
    // Attempt to load previously saved playlist
    loadPlaylist();
}

UIPlaylist::~UIPlaylist() {
    // Save playlist on destruction
    savePlaylist();
}

void UIPlaylist::render() {
    ImGui::Begin("Playlist");

    renderPlaylistControls();
    renderPlaylistItems();

    ImGui::End();
}

void UIPlaylist::renderPlaylistControls() {
    // Add video button
    if (ImGui::Button("Add Video")) {
        // TODO: Implement file dialog
        ImGui::OpenPopup("Add Video");
    }
    ImGui::SameLine();

    // Clear playlist button
    if (ImGui::Button("Clear Playlist")) {
        clearPlaylist();
    }
    ImGui::SameLine();

    // Shuffle and repeat toggles
    ImGui::Checkbox("Shuffle", &shuffleMode);
    ImGui::SameLine();
    ImGui::Checkbox("Repeat", &repeatMode);
}

void UIPlaylist::renderPlaylistItems() {
    ImGui::BeginChild("Playlist Items", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    
    for (int i = 0; i < items.size(); ++i) {
        ImGui::PushID(i);
        
        // Selectable item with context menu
        bool isSelected = (i == currentItemIndex);
        if (ImGui::Selectable(items[i].filename.c_str(), &isSelected, 
                               ImGuiSelectableFlags_AllowDoubleClick)) {
            // Double-click to play
            if (ImGui::IsMouseDoubleClicked(0)) {
                currentItemIndex = i;
                // TODO: Trigger video playback
            }
        }

        // Drag and drop reordering
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("PLAYLIST_ITEM", &i, sizeof(int));
            ImGui::Text("Move %s", items[i].filename.c_str());
            ImGui::EndDragDropSource();
        }
        
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PLAYLIST_ITEM")) {
                int payloadIndex = *(const int*)payload->Data;
                moveItem(payloadIndex, i);
            }
            ImGui::EndDragDropTarget();
        }

        // Context menu
        if (ImGui::BeginPopupContextItem("item context menu")) {
            if (ImGui::MenuItem("Remove")) {
                removeItem(i);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }
    
    ImGui::EndChild();
}

void UIPlaylist::addItem(const std::string& filepath) {
    // Validate file exists and is a supported video format
    if (!std::filesystem::exists(filepath) || 
        !MediaPlayerConfig::isSupportedVideoFormat(filepath)) {
        std::cerr << "Invalid video file: " << filepath << std::endl;
        return;
    }

    PlaylistItem newItem;
    newItem.filepath = filepath;
    newItem.filename = std::filesystem::path(filepath).filename().string();
    newItem.isCurrentlyPlaying = false;

    // Extract media metadata
    extractMediaMetadata(newItem);

    items.push_back(newItem);
}

void UIPlaylist::removeItem(int index) {
    if (index < 0 || index >= items.size()) return;

    items.erase(items.begin() + index);

    // Adjust current item index if necessary
    if (currentItemIndex >= items.size()) {
        currentItemIndex = items.empty() ? -1 : items.size() - 1;
    }
}

void UIPlaylist::clearPlaylist() {
    items.clear();
    currentItemIndex = -1;
}

void UIPlaylist::moveItem(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= items.size() || 
        toIndex < 0 || toIndex >= items.size() || 
        fromIndex == toIndex) return;

    PlaylistItem item = items[fromIndex];
    items.erase(items.begin() + fromIndex);
    items.insert(items.begin() + toIndex, item);

    // Update current item index if affected
    if (currentItemIndex == fromIndex) {
        currentItemIndex = toIndex;
    } else if (currentItemIndex >= std::min(fromIndex, toIndex) && 
               currentItemIndex <= std::max(fromIndex, toIndex)) {
        currentItemIndex += (fromIndex < toIndex) ? -1 : 1;
    }
}

void UIPlaylist::playNext() {
    if (items.empty()) return;

    if (shuffleMode) {
        // Random selection
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, items.size() - 1);
        currentItemIndex = dis(gen);
    } else {
        // Linear progression
        currentItemIndex = (currentItemIndex + 1) % items.size();
    }

    // Mark current item as playing
    for (auto& item : items) {
        item.isCurrentlyPlaying = false;
    }
    items[currentItemIndex].isCurrentlyPlaying = true;

    // TODO: Trigger video playback
}

void UIPlaylist::playPrevious() {
    if (items.empty()) return;

    currentItemIndex = (currentItemIndex - 1 + items.size()) % items.size();

    // Mark current item as playing
    for (auto& item : items) {
        item.isCurrentlyPlaying = false;
    }
    items[currentItemIndex].isCurrentlyPlaying = true;

    // TODO: Trigger video playback
}

void UIPlaylist::shuffle() {
    if (items.size() <= 1) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(items.begin(), items.end(), gen);
}

void UIPlaylist::toggleRepeat() {
    repeatMode = !repeatMode;
}

void UIPlaylist::extractMediaMetadata(PlaylistItem& item) {
    AVFormatContext* formatContext = nullptr;
    
    // Open input file
    if (avformat_open_input(&formatContext, item.filepath.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Could not open input file: " << item.filepath << std::endl;
        return;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        avformat_close_input(&formatContext);
        std::cerr << "Could not find stream information: " << item.filepath << std::endl;
        return;
    }

    // Get duration
    item.duration = formatContext->duration > 0 ? 
                    formatContext->duration / static_cast<double>(AV_TIME_BASE) : 0.0;

    // Clean up
    avformat_close_input(&formatContext);
}

void UIPlaylist::savePlaylist() {
    // Default playlist save path
    std::string playlistPath = "playlist.m3u";
    saveToFile(playlistPath);
}

void UIPlaylist::loadPlaylist() {
    // Default playlist load path
    std::string playlistPath = "playlist.m3u";
    loadFromFile(playlistPath);
}

bool UIPlaylist::saveToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open playlist file for writing: " << filepath << std::endl;
        return false;
    }

    // Write playlist header
    file << "#EXTM3U\n";

    // Write each playlist item
    for (const auto& item : items) {
        file << "#EXTINF:" << static_cast<int>(item.duration) 
             << "," << item.filename << "\n";
        file << item.filepath << "\n";
    }

    return true;
}

bool UIPlaylist::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open playlist file: " << filepath << std::endl;
        return false;
    }

    // Clear existing playlist
    clearPlaylist();

    std::string line;
    // Skip M3U header
    std::getline(file, line);

    while (std::getline(file, line)) {
        // Skip extended info lines
        if (line.starts_with("#EXTINF:")) continue;

        // Add video file to playlist
        if (!line.empty() && std::filesystem::exists(line)) {
            addItem(line);
        }
    }

    return true;
}

const std::vector<UIPlaylist::PlaylistItem>& UIPlaylist::getItems() const {
    return items;
}

UIPlaylist::PlaylistItem UIPlaylist::getCurrentItem() const {
    return (currentItemIndex >= 0 && currentItemIndex < items.size()) 
        ? items[currentItemIndex] 
        : PlaylistItem{};
}

bool UIPlaylist::isEmpty() const {
    return items.empty();
}

int UIPlaylist::getCurrentItemIndex() const {
    return currentItemIndex;
}
