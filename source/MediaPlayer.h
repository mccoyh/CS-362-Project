#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <audio.h>
#include <AudioToTxt.h>
#include <AVParser.h>
#include <VulkanEngine.h>
#include <memory>

constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
  .WINDOW_WIDTH = 1000,
  .WINDOW_HEIGHT = 600,
  .WINDOW_TITLE = "Medos"
};

class MediaPlayer {
public:
  explicit MediaPlayer(const char* asset);

  ~MediaPlayer();

  void run();

private:
  const char* asset;

  std::unique_ptr<VkEngine::VulkanEngine> vulkanEngine;

  std::unique_ptr<AVParser::MediaParser> parser;

  std::unique_ptr<Captions::CaptionCache> captionCache{};

  Audio::AudioData audioData{};

  uint32_t audioDurationRemaining = 0;

  void loadCaptions() const;

  void update();

  void handleKeyInput();

  void displayGui();

  void menuBarGui();

  void timelineGui();

  void volumeGui();

  void navigateFrames(int numFrames) const;
};

#endif //MEDIAPLAYER_H
