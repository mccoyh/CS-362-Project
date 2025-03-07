project("sfx")

add_executable(${PROJECT_NAME} main.cpp
  ../VideoDecoder.cpp
  ../VideoDecoder.h
)

if (UNIX)
  message("Linking RPATH on LINUX")

  set_target_properties(sfx PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib"
  )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
  VulkanEngine
  FFmpeg::avformat
  FFmpeg::avcodec
  FFmpeg::swscale
  FFmpeg::avutil
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

# Display all linked libraries
get_target_property(linked_libs ${PROJECT_NAME} LINK_LIBRARIES)
message(STATUS "Target ${PROJECT_NAME} links to: ${linked_libs}")