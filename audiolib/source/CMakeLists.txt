project(audiolib)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
  audio.cpp
  audio.h
)

# Load SDL3
if (WIN32)
  set(SDL_AUDIO ON CACHE BOOL "Enable SDL Audio" FORCE)

  # Disable features so that it interacts nicely with windows
  set(SDL_VIDEO OFF CACHE BOOL "Disable SDL Video" FORCE)
  set(SDL_GPU OFF CACHE BOOL "Disable SDL GPU" FORCE)
  set(SDL_RENDER OFF CACHE BOOL "Disable SDL Render" FORCE)
  set(SDL_CAMERA OFF CACHE BOOL "Disable SDL Camera" FORCE)
  set(SDL_JOYSTICK OFF CACHE BOOL "Disable SDL Joystick" FORCE)
  set(SDL_HAPTIC OFF CACHE BOOL "Disable SDL Haptic" FORCE)
  set(SDL_HIDAPI OFF CACHE BOOL "Disable SDL HIDAPI" FORCE)
  set(SDL_POWER OFF CACHE BOOL "Disable SDL Power" FORCE)
  set(SDL_SENSOR OFF CACHE BOOL "Disable SDL Sensor" FORCE)
  set(SDL_DIALOG OFF CACHE BOOL "Disable SDL Dialog" FORCE)
endif ()

FetchContent_Declare(SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.4
)

FetchContent_MakeAvailable(SDL3)

# Specify the components FFmpeg should have available
set(FFMPEG_COMPONENTS avcodec avformat swscale avutil)

# Find FFmpeg package
find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES} SDL3::SDL3)
target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS} ${SDL3_INCLUDE_DIRS})

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