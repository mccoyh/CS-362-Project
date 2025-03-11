list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil swresample)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

# Load libraries
add_subdirectory(vulkanEngine)

add_subdirectory(audiolib)

add_subdirectory(AudioToTxt)

add_subdirectory(avParser)