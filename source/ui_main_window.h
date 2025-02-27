#pragma once

#include <imgui.h>
#include "ui_video_player.h"
#include "ui_playlist.h"
#include "ui_subtitle_controls.h"
// #include "ui_video_editor.h"
#include "ui_settings.h"
#include "ui_file_explorer.h"

class UIMainWindow {
private:
    // UI Components
    UIVideoPlayer videoPlayer;
    UIPlaylist playlist;
    UISubtitleControls subtitleControls;
    // UIVideoEditor videoEditor;
    UISettings settings;
    UIFileExplorer fileExplorer;

    // Current active tab
    enum class ActiveTab {
        VideoPlayer,
        Playlist,
        VideoEditor,
        Settings,
        FileExplorer
    } currentTab;

    // Application theme
    void applyTheme();

    // Main menu bar
    void renderMenuBar();

    // Render current active tab
    void renderActiveTab();

public:
    UIMainWindow();

    // Main rendering method
    void render();
};