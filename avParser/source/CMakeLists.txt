project(AVParser)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
  AVParser.cpp
  AVParser.h
)

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS})

# Create Include Headers
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${CMAKE_SOURCE_DIR}/avParser/source)
  file(COPY
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/avParser
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "include/*" EXCLUDE
  )
endif()