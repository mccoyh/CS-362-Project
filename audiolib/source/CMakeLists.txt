project(audiolib)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
  audio.cpp
  audio.h
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(SDL2Test Main.cpp)
target_link_libraries(SDL2Test ${SDL2_LIBRARIES})

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

# Create Include Headers
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${CMAKE_SOURCE_DIR}/audiolib/source)
  file(COPY
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/audiolib
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "include/*" EXCLUDE
  )
endif()