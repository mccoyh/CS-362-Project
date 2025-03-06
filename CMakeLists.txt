cmake_minimum_required(VERSION 3.12)
project(DesktopMediaPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

# Find required packages
find_package(FFmpeg REQUIRED COMPONENTS avcodec avformat swscale avutil)
find_package(VulkanEngine REQUIRED)
find_package(ImGui REQUIRED)

# Add executable
add_executable(${PROJECT_NAME}
    main.cpp
    ui_file_explorer.cpp
    ui_file_explorer.h
    ui_main_window.cpp
    ui_main_window.h
    ui_playlist.cpp
    ui_playlist.h
    ui_settings.cpp
    ui_settings.h
    ui_subtitle_controls.cpp
    ui_subtitle_controls.h
    ui_video_player.cpp
    ui_video_player.h
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    VulkanEngine 
    ${FFMPEG_LIBRARIES}
    ImGui
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${VulkanEngine_INCLUDE_DIRS} 
    ${FFMPEG_INCLUDE_DIRS}
)
