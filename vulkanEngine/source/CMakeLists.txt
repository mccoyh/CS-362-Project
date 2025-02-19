project(VulkanEngine)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_library(${PROJECT_NAME}
  VulkanEngine.cpp
  VulkanEngine.h
  components/Instance.cpp
  components/Instance.h
  components/DebugMessenger.cpp
  components/DebugMessenger.h
  components/Window.cpp
  components/Window.h
  components/PhysicalDevice.cpp
  components/PhysicalDevice.h
  components/LogicalDevice.cpp
  components/LogicalDevice.h
  utilities/Buffers.cpp
  utilities/Buffers.h
  utilities/Images.cpp
  utilities/Images.h
  pipelines/RenderPass.cpp
  pipelines/RenderPass.h
  components/SwapChain.cpp
  components/SwapChain.h
  components/Framebuffer.cpp
  components/Framebuffer.h
  VulkanEngineOptions.h
  pipelines/ShaderModule.cpp
  pipelines/ShaderModule.h
  pipelines/Pipeline.cpp
  pipelines/Pipeline.h
  pipelines/GraphicsPipeline.cpp
  pipelines/GraphicsPipeline.h
)

# Vulkan
find_package(Vulkan REQUIRED)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

# imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(imgui PUBLIC
  Vulkan::Vulkan
  glfw
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
)

# Include
FetchContent_MakeAvailable(
  glfw
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Vulkan::Vulkan
  glfw
  imgui
)

# Create Include Headers
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL ${CMAKE_SOURCE_DIR}/vulkanEngine/source)
  file(COPY
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/VulkanEngine
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "include/*" EXCLUDE
  )
endif()