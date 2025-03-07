project("videoDecode")

add_executable(${PROJECT_NAME} main.cpp
  ../VideoDecoder.cpp
  ../VideoDecoder.h
)
message("FFMPEG LIBRARIES TO LINK:")
message("${FFMPEG_LIBRARIES}")
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanEngine ${FFMPEG_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})