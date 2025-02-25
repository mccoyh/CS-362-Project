project(AudioToTxt)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

# User option to enable/disable Vulkan (default: ON)
option(WHISPER_USE_VULKAN "Enable Vulkan support for Whisper" ON)

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

# Define lists of model URLs and paths
set(MODEL_URLS
    "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo-q5_0.bin?download=true"
    "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin?download=true"
)

set(MODEL_PATHS
    "${CMAKE_BINARY_DIR}/models/ggml-large-v3-turbo-q5_0.bin"
    "${CMAKE_BINARY_DIR}/models/ggml-base.bin"
)
list(LENGTH MODEL_URLS num_models)

# Loop through the lists and download models if they don't exist
foreach(i RANGE 0 ${num_models} - 1) 
    list(GET MODEL_URLS ${i} model_url)
    list(GET MODEL_PATHS ${i} model_path)

    if(NOT EXISTS ${model_path})
        message(STATUS "Downloading model from ${model_url}...")
        file(DOWNLOAD ${model_url} ${model_path} SHOW_PROGRESS)
    endif()
endforeach()


# Check for Vulkan if enabled
if (WHISPER_USE_VULKAN)
    find_package(Vulkan)

    if (Vulkan_FOUND)
        message(STATUS "Vulkan found, enabling Vulkan support for Whisper")
        # Set GGML_VULKAN and other related flags to ON
        set(GGML_USE_VULKAN ON CACHE BOOL "Enable Vulkan in GGML" FORCE)
        set(GGML_VULKAN ON CACHE BOOL "Enable Vulkan in GGML" FORCE)
        
        # Set Whisper backend to Vulkan
        set(WHISPER_BACKEND "Vulkan" CACHE STRING "Set Whisper backend" FORCE)
    else()
        message(WARNING "Vulkan not found, building Whisper without Vulkan support")
        set(GGML_VULKAN OFF CACHE BOOL "Disable Vulkan in GGML" FORCE)
        set(GGML_USE_VULKAN OFF CACHE BOOL "Disable Vulkan in GGML" FORCE)
        set(WHISPER_BACKEND "CPU" CACHE STRING "Set Whisper backend" FORCE)
        set(WHISPER_USE_VULKAN OFF)
    endif()
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


#Vulkan
find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    whisper
)

if (WHISPER_USE_VULKAN)
    target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
endif()

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