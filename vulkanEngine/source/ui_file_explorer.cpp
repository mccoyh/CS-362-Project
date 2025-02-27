#include "ui_file_explorer.h"
#include "../include/config.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

// Supported video extensions
const std::vector<std::string> UIFileExplorer::SUPPORTED_VIDEO_EXTENSIONS = {
    ".mp4", ".avi", ".mkv", ".mov", ".wmv", 
    ".flv", ".webm", ".m4v", ".mpg", ".mpeg"
};

// Supported subtitle extensions
const std::vector<std::string> UIFileExplorer::SUPPORTED_SUBTITLE_EXTENSIONS = {
    ".srt", ".vtt", ".ass", ".sub", ".idx"
};

UIFileExplorer::UIFileExplorer() : 
    currentPath(std::filesystem::current_path()),
    currentSortMethod(SortBy::Name),
    sortAscending(true),
    showVideosOnly(false),
    showSubtitlesOnly(false) {
    
    // Add current path to recent paths
    addToRecentPaths(currentPath);
    
    // Initial directory contents
    refreshDirectoryContents();
}

void UIFileExplorer::render() {
    ImGui::Begin("File Explorer");

    renderPathNavigation();
    renderFilterOptions();
    renderFileList();

    ImGui::End();
}

void UIFileExplorer::renderPathNavigation() {
    // Current path display
    ImGui::Text("Current Path: %s", currentPath.string().c_str());
    
    // Navigation buttons
    if (ImGui::Button("Parent Directory")) {
        currentPath = currentPath.parent_path();
        refreshDirectoryContents();
    }
    ImGui::SameLine();
    if (ImGui::Button("Home")) {
        currentPath = std::filesystem::home_directory();
        refreshDirectoryContents();
    }
    ImGui::SameLine();
    if (ImGui::Button("New Folder")) {
        ImGui::OpenPopup("Create Directory");
    }

    // Create directory popup
    if (ImGui::BeginPopupModal("Create Directory", nullptr, 
                                ImGuiWindowFlags_AlwaysAutoResize)) {
        static char dirName[256] = "";
        ImGui::InputText("Directory Name", dirName, IM_ARRAYSIZE(dirName));
        
        if (ImGui::Button("Create")) {
            if (strlen(dirName) > 0) {
                createDirectory(dirName);
                memset(dirName, 0, sizeof(dirName));
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UIFileExplorer::renderFilterOptions() {
    // Filtering options
    ImGui::Checkbox("Videos Only", &showVideosOnly);
    ImGui::SameLine();
    ImGui::Checkbox("Subtitles Only", &showSubtitlesOnly);

    // Sorting options
    ImGui::Text("Sort By:");
    ImGui::SameLine();
    const char* sortOptions[] = {"Name", "Size", "Modified"};
    int currentSort = static_cast<int>(currentSortMethod);
    if (ImGui::Combo("##SortMethod", &currentSort, sortOptions, IM_ARRAYSIZE(sortOptions))) {
        currentSortMethod = static_cast<SortBy>(currentSort);
        sortEntries();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Ascending", &sortAscending);
}

void UIFileExplorer::renderFileList() {
    ImGui::BeginChild("FileList", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2));
    
    // Filter entries
    filterEntries();

    for (const auto& entry : currentEntries) {
        ImGui::PushID(entry.name.c_str());
        
        // Render entry
        bool isSelected = false;
        if (ImGui::Selectable(entry.name.c_str(), &isSelected, 
                               ImGuiSelectableFlags_AllowDoubleClick)) {
            // Double-click to enter directory or open file
            if (entry.isDirectory && ImGui::IsMouseDoubleClicked(0)) {
                currentPath /= entry.name;
                refreshDirectoryContents();
            }
        }

        // Context menu
        renderContextMenu(entry);

        // File info tooltip
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Full Path: %s", entry.fullPath.string().c_str());
            ImGui::Text("Size: %s", 
                FileSystemUtils::formatFileSize(entry.size).c_str());
            ImGui::Text("Last Modified: %s", 
                FileSystemUtils::formatFileTime(entry.lastModified).c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
}

void UIFileExplorer::renderContextMenu(const FileEntry& entry) {
    if (ImGui::BeginPopupContextItem("file_context_menu")) {
        if (ImGui::MenuItem("Open")) {
            if (entry.isDirectory) {
                currentPath /= entry.name;
                refreshDirectoryContents();
            } else {
                // TODO: Implement file opening mechanism
                std::cout << "Opening file: " << entry.fullPath << std::endl;
            }
        }
        
        if (!entry.isDirectory) {
            if (ImGui::MenuItem("Rename")) {
                // TODO: Implement renaming
                ImGui::OpenPopup("Rename File");
            }
            if (ImGui::MenuItem("Delete")) {
                deleteFile(entry.fullPath);
            }
        }
        ImGui::EndPopup();
    }
}

void UIFileExplorer::refreshDirectoryContents() {
    currentEntries.clear();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
            FileEntry fileEntry;
            fileEntry.name = entry.path().filename().string();
            fileEntry.fullPath = entry.path();
            fileEntry.isDirectory = entry.is_directory();
            
            if (!fileEntry.isDirectory) {
                fileEntry.size = std::filesystem::file_size(entry.path());
            } else {
                fileEntry.size = 0;
            }

            fileEntry.lastModified = std::filesystem::last_write_time(entry.path());

            currentEntries.push_back(fileEntry);
        }

        // Sort entries
        sortEntries();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
}

void UIFileExplorer::sortEntries() {
    std::sort(currentEntries.begin(), currentEntries.end(), 
        [this](const FileEntry& a, const FileEntry& b) {
            // Directories always come first
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory;
            }

            // Apply sorting based on current method
            bool result = false;
            switch (currentSortMethod) {
                case SortBy::Name:
                    result = a.name < b.name;
                    break;
                case SortBy::Size:
                    result = a.size < b.size;
                    break;
                case SortBy::Modified:
                    result = a.lastModified < b.lastModified;
                    break;
            }

            // Respect ascending/descending
            return sortAscending ? result : !result;
        }
    );
}

void UIFileExplorer::filterEntries() {
    // Remove entries that don't match filter criteria
    currentEntries.erase(
        std::remove_if(currentEntries.begin(), currentEntries.end(), 
            [this](const FileEntry& entry) {
                // Skip directories
                if (entry.isDirectory) return false;

                // Apply video filter
                if (showVideosOnly) {
                    return !isVideoFile(entry.fullPath);
                }

                // Apply subtitle filter
                if (showSubtitlesOnly) {
                    return !isSubtitleFile(entry.fullPath);
                }

                // No filter applied
                return false;
            }
        ), 
        currentEntries.end()
    );
}

void UIFileExplorer::createDirectory(const std::string& name) 
    try {
        std::filesystem::path newDirPath = currentPath / name;
        
        if (std::filesystem::exists(newDirPath)) {
            std::cerr << "Directory already exists: " << newDirPath << std::endl;
            return;
        }

        std::filesystem::create_directory(newDirPath);
        refreshDirectoryContents();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        }

void UIFileExplorer::renameFile(const std::filesystem::path& oldPath, 
                                const std::filesystem::path& newPath) {
    try {
        std::filesystem::rename(oldPath, newPath);
        refreshDirectoryContents();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
    }
}

void UIFileExplorer::deleteFile(const std::filesystem::path& filepath) {
    try {
        std::filesystem::remove(filepath);
        refreshDirectoryContents();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
    }
}

void UIFileExplorer::openFileDialog() {
    // TODO: Implement platform-specific file dialog
    // This is a placeholder that opens the current directory
    std::cout << "Open File Dialog - Current Path: " 
              << currentPath.string() << std::endl;
}

void UIFileExplorer::selectDirectory() {
    // TODO: Implement platform-specific directory selection dialog
    std::cout << "Select Directory Dialog - Current Path: " 
              << currentPath.string() << std::endl;
}

void UIFileExplorer::addToRecentPaths(const std::filesystem::path& path) {
    // Prevent duplicates
    auto it = std::find(recentPaths.begin(), recentPaths.end(), path);
    if (it != recentPaths.end()) {
        recentPaths.erase(it);
    }

    // Add to front of list
    recentPaths.insert(recentPaths.begin(), path);

    // Limit recent paths to 10
    if (recentPaths.size() > 10) {
        recentPaths.pop_back();
    }
}

std::filesystem::path UIFileExplorer::getCurrentPath() const {
    return currentPath;
}

std::vector<std::filesystem::path> UIFileExplorer::getRecentPaths() const {
    return recentPaths;
}

bool UIFileExplorer::isFileSupported(const std::filesystem::path& filepath) const {
    return isVideoFile(filepath) || isSubtitleFile(filepath);
}

bool UIFileExplorer::isVideoFile(const std::filesystem::path& filepath) {
    std::string ext = filepath.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(SUPPORTED_VIDEO_EXTENSIONS.begin(), 
                     SUPPORTED_VIDEO_EXTENSIONS.end(), 
                     ext) != SUPPORTED_VIDEO_EXTENSIONS.end();
}

bool UIFileExplorer::isSubtitleFile(const std::filesystem::path& filepath) {
    std::string ext = filepath.extension().string();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(SUPPORTED_SUBTITLE_EXTENSIONS.begin(), 
                     SUPPORTED_SUBTITLE_EXTENSIONS.end(), 
                     ext) != SUPPORTED_SUBTITLE_EXTENSIONS.end();
}

// File System Utilities Implementation
std::string FileSystemUtils::formatFileSize(uintmax_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        ++unitIndex;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return oss.str();
}

std::string FileSystemUtils::formatFileTime(
    const std::filesystem::file_time_type& time
) {
    // Convert to time_t
    auto timeT = std::filesystem::file_time_type::clock::to_time_t(time);
    
    // Convert to local time
    std::tm* localTime = std::localtime(&timeT);
    
    // Format time
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool FileSystemUtils::canRead(const std::filesystem::path& filepath) {
    std::error_code ec;
    return std::filesystem::status_known(std::filesystem::status(filepath, ec)) 
           && std::filesystem::perms::owner_read == 
              (std::filesystem::status(filepath, ec).permissions() & 
               std::filesystem::perms::owner_read);
}

bool FileSystemUtils::canWrite(const std::filesystem::path& filepath) {
    std::error_code ec;
    return std::filesystem::status_known(std::filesystem::status(filepath, ec)) 
           && std::filesystem::perms::owner_write == 
              (std::filesystem::status(filepath, ec).permissions() & 
               std::filesystem::perms::owner_write);
}

bool FileSystemUtils::canExecute(const std::filesystem::path& filepath) {
    std::error_code ec;
    return std::filesystem::status_known(std::filesystem::status(filepath, ec)) 
           && std::filesystem::perms::owner_exec == 
              (std::filesystem::status(filepath, ec).permissions() & 
               std::filesystem::perms::owner_exec);
}