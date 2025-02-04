cmake_minimum_required(VERSION 3.2)
project(CS_362_Project)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

add_subdirectory(vulkanEngine)

add_subdirectory(source)
