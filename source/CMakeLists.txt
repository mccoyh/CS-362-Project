project(DesktopMediaPlayer)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES
  BUILD_WITH_INSTALL_RPATH TRUE
  INSTALL_RPATH "$ORIGIN"
  SKIP_BUILD_RPATH FALSE
)