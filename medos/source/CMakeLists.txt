project(Medos)

add_executable(${PROJECT_NAME}
  main.cpp
  ../../AudioToTxt/tests/test_whisper/audioDecoding.h
  ../../AudioToTxt/tests/test_whisper/audioDecoding.cpp
  MediaPlayer.cpp
  MediaPlayer.h
)

# Fetch ImGuiFileBrowser
FetchContent_Declare(
    imguifilebrowser
    GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
    GIT_TAG master # Use a stable tag instead of master if needed
)
FetchContent_MakeAvailable(imguifilebrowser)
target_include_directories(${PROJECT_NAME} PRIVATE ${imguifilebrowser_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
  audiolib
  AudioToTxt
  AVParser
  VulkanEngine
)