project(avExtraction)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
  VulkanEngine
  FFmpeg::avformat
  FFmpeg::avcodec
  FFmpeg::swscale
  FFmpeg::avutil
)