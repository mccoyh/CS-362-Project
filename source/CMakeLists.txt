project(DesktopMediaPlayer)
include(FetchContent)

set(BUILD_SHARED_LIBS ON)

add_executable(${PROJECT_NAME}
  main.cpp
)