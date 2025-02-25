project(AudioToTxt)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
    AudioToTxt.h
    AudioToTxt.cpp
)

#turbo model
set(MODEL_URL "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo-q5_0.bin?download=true")
set(MODEL_PATH "${CMAKE_BINARY_DIR}/models/ggml-large-v3-turbo-q5_0.bin")

#base model
set(MODEL_URL_BASE "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin?download=true")
set(MODEL_PATH_BASE "${CMAKE_BINARY_DIR}/models/ggml-base.bin")

# Download model if it doesn't exist
if(NOT EXISTS ${MODEL_PATH})
    message(STATUS "Downloading Whisper model...")
    file(DOWNLOAD ${MODEL_URL} ${MODEL_PATH} SHOW_PROGRESS)
endif()
#download base model
if(NOT EXISTS ${MODEL_PATH_BASE})
    message(STATUS "Downloading Whisper model...")
    file(DOWNLOAD ${MODEL_URL_BASE} ${MODEL_PATH_BASE} SHOW_PROGRESS)
endif()

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



target_link_libraries(${PROJECT_NAME} PUBLIC 
    Vulkan::Vulkan
    whisper
)
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