project("sfx")

add_executable(${PROJECT_NAME} main.cpp
  ../VideoDecoder.cpp
  ../VideoDecoder.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE VulkanEngine ${FFMPEG_COMPONENTS})

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})