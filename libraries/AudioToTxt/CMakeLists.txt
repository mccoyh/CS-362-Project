include_directories("${CMAKE_SOURCE_DIR}/AudioToTxt")

add_subdirectory(source)

add_subdirectory(tests)

# Path to the assets directory (relative to this cmake file)
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# Copy assets to the build directory at configure time
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})