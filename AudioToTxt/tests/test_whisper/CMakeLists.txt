project("test_whisper")

add_executable(${PROJECT_NAME} main.cpp audioDecoding.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC AudioToTxt)

set(FFMPEG_COMPONENTS avcodec avformat avutil swscale swresample)

find_package(FFmpeg REQUIRED COMPONENTS ${FFMPEG_COMPONENTS})

target_link_libraries(${PROJECT_NAME} PUBLIC 
    ${FFMPEG_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS})
