#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <imgui.h>

class UIPlaylist {
public:
    // Playlist item structure
    struct PlaylistItem {
        std::string filepath;
        std::string filename;
        double duration;
        bool isCurrentlyPlaying;
    };

private:
    // Playlist management
    std::vector<PlaylistItem> items;
    int currentItemIndex;
    bool shuffleMode;
    bool repeatMode;

    // Playlist file operations
    void savePlaylist();
    void loadPlaylist();

    // Metadata extraction
    void extractMediaMetadata(PlaylistItem& item);

    // Rendering helpers
    void renderPlaylistControls();
    void renderPlaylistItems();

public:
    UIPlaylist();
    ~UIPlaylist();

    // Main rendering method
    void render();

    // Playlist manipulation methods
    void addItem(const std::string& filepath);
    void removeItem(int index);
    void clearPlaylist();
    void moveItem(int fromIndex, int toIndex);

    // Playback control methods
    void playNext();
    void playPrevious();
    void shuffle();
    void toggleRepeat();

    // Accessors
    const std::vector<PlaylistItem>& getItems() const;
    PlaylistItem getCurrentItem() const;
    bool isEmpty() const;
    int getCurrentItemIndex() const;

    // Save/Load playlist
    bool saveToFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath);
};