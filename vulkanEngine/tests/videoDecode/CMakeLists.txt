project("videoDecode")
include(FetchContent)

add_executable(${PROJECT_NAME} main.cpp
  ../VideoDecoder.cpp
  ../VideoDecoder.h
)

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PRIVATE VulkanEngine ${FFMPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})