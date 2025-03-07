project("test_whisper")

add_executable(${PROJECT_NAME} main.cpp audioDecoding.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE AudioToTxt)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${FFMPEG_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
