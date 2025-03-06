project("test_whisper")

add_executable(${PROJECT_NAME} main.cpp audioDecoding.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE AudioToTxt)

set(FFMPEG_COMPONENTS avcodec avformat avutil swscale swresample)

find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

if (UNIX AND NOT APPLE)
  # Use the imported targets instead of manual linking
  foreach(component ${FFMPEG_COMPONENTS})
    target_link_libraries(${PROJECT_NAME} PRIVATE FFmpeg::${component})
  endforeach()
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC
      ${FFMPEG_LIBRARIES}
  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
