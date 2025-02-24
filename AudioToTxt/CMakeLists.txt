include_directories("${CMAKE_SOURCE_DIR}/AudioToTxt")

add_subdirectory(source)

add_subdirectory(tests)

# Path to the assets directory (relative to the project root)
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/AudioToTxt/assets")

# Define where to copy assets (inside the build directory)
set(BUILD_ASSETS_DIR "${CMAKE_BINARY_DIR}/assets")

# Copy assets to the build directory at configure time
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_BINARY_DIR})