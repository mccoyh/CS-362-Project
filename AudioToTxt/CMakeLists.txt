include_directories("${CMAKE_SOURCE_DIR}/AudioToTxt")

add_subdirectory(source)

add_subdirectory(tests)

# Path to the assets directory (relative to the project root)
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/AudioToTxt/assets")

# Copy assets to the build directory at configure time
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})