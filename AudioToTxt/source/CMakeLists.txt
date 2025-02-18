project(AudioToTxt)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
    AudioToTxt.h
    AudioToTxt.cpp
)

#Whisper ASR CPP
FetchContent_Declare(
    whisper
    GIT_REPOSITORY https://github.com/ggerganov/whisper.cpp
    GIT_TAG v1.7.4
)

FetchContent_MakeAvailable(
    whisper
)

option(GGML_VULKAN "Enable Vulkan Support" ON)

#Vulkan
find_package(Vulkan REQUIRED)

#FFmpeg
set(FFMPEG_COMPONENTS avcodec avformat avutil)

find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})



target_link_libraries(${PROJECT_NAME} PUBLIC 
    ${FFMPEG_LIBRARIES}
    Vulkan::Vulkan
    whisper
)
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

#Create Include Headers
IF (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${CMAKE_SOURCE_DIR}/AudioToTxt/source)
    file(COPY
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/AudioToTxt
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "include/*" EXCLUDE
    )
endif()