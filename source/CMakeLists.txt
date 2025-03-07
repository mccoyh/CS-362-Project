project(Medos)

add_executable(${PROJECT_NAME}
  main.cpp
  ../AudioToTxt/tests/test_whisper/audioDecoding.h
  ../AudioToTxt/tests/test_whisper/audioDecoding.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  audiolib
  AudioToTxt
  AVParser
  VulkanEngine
)