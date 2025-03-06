cmake_minimum_required(VERSION 3.10)
project(avExtraction)

# Add executable
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    VulkanEngine 
    AVParser
    ImGui
)
