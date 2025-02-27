project(DesktopMediaPlayer)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

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
#  ui_video_editor.cpp
#  ui_video_editor.h
  ui_video_player.cpp
  ui_video_player.h
)

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PRIVATE VulkanEngine ${FFMPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${VulkanEngine_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})