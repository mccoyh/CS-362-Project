// config.h

#ifndef CONFIG_H
#define CONFIG_H

// Project Information
#define PROJECT_NAME "CS-362 Project"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

// Feature Flags
#define USE_FFMPEG 1
#define USE_VULKAN 1

// UI Components
#define ENABLE_FILE_EXPLORER 1
#define ENABLE_MAIN_WINDOW 1
#define ENABLE_PLAYLIST 1
#define ENABLE_SETTINGS 1
#define ENABLE_SUBTITLE_CONTROLS 1
#define ENABLE_VIDEO_EDITOR 1
#define ENABLE_VIDEO_PLAYER 1

// Debug Options
#ifdef DEBUG
    #define ENABLE_LOGGING 1
    #define LOG_LEVEL 3 // 1=Error, 2=Warning, 3=Info, 4=Debug
#else
    #define ENABLE_LOGGING 0
#endif

#endif // CONFIG_H