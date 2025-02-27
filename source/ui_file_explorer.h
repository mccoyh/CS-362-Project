#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <imgui.h>

class UIFileExplorer {
public:
    // File entry structure
    struct FileEntry {
        std::string name;
        std::filesystem::path fullPath;
        bool isDirectory;
        uintmax_t size;
        std::filesystem::file_time_type lastModified;
    };

    // Sorting options
    enum class SortBy {
        Name,
        Size,
        Modified
    };

private:
    // Current directory being explored
    std::filesystem::path currentPath;

    // List of entries in current directory
    std::vector<FileEntry> currentEntries;

    // Supported video file extensions
    static const std::vector<std::string> SUPPORTED_VIDEO_EXTENSIONS;

    // Supported subtitle file extensions
    static const std::vector<std::string> SUPPORTED_SUBTITLE_EXTENSIONS;

    // Sorting and filtering
    SortBy currentSortMethod;
    bool sortAscending;
    bool showVideosOnly;
    bool showSubtitlesOnly;

    // Recent paths
    std::vector<std::filesystem::path> recentPaths;

    // Helper methods
    void refreshDirectoryContents();
    void sortEntries();
    void filterEntries();
    bool isFileSupported(const std::filesystem::path& filepath) const;
    void addToRecentPaths(const std::filesystem::path& path);

    // Rendering helper methods
    void renderPathNavigation();
    void renderFilterOptions();
    void renderFileList();
    void renderContextMenu(const FileEntry& entry);

public:
    UIFileExplorer();

    // Main rendering method
    void render();

    // File operation methods
    void openFileDialog();
    void selectDirectory();
    void createDirectory(const std::string& name);
    void renameFile(const std::filesystem::path& oldPath, 
                    const std::filesystem::path& newPath);
    void deleteFile(const std::filesystem::path& filepath);

    // Accessors
    std::filesystem::path getCurrentPath() const;
    std::vector<std::filesystem::path> getRecentPaths() const;

    // Static utility methods
    static bool isVideoFile(const std::filesystem::path& filepath);
    static bool isSubtitleFile(const std::filesystem::path& filepath);
};

// File system utility class
class FileSystemUtils {
public:
    // Get file size as a human-readable string
    static std::string formatFileSize(uintmax_t bytes);

    // Get file modification time as a readable string
    static std::string formatFileTime(
        const std::filesystem::file_time_type& time
    );

    // Check file permissions
    static bool canRead(const std::filesystem::path& filepath);
    static bool canWrite(const std::filesystem::path& filepath);
    static bool canExecute(const std::filesystem::path& filepath);
};