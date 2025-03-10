#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <audio.h>
#include <AudioToTxt.h>
#include <AVParser.h>
#include <VulkanEngine.h>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "imfilebrowser.h"

constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
  .WINDOW_WIDTH = 1000,
  .WINDOW_HEIGHT = 600,
  .WINDOW_TITLE = "Medos"
};

constexpr VkEngine::VulkanEngineOptions fullscreenVulkanEngineOptions {
  .FULLSCREEN = true,
  .WINDOW_TITLE = "Medos"
};

class MediaPlayer {
public:
  explicit MediaPlayer(const char* asset);

  ~MediaPlayer();

  void run();

private:
  const char* asset;

  ImGui::FileBrowser fileDialog;  

  std::unique_ptr<VkEngine::VulkanEngine> vulkanEngine;

  std::unique_ptr<AVParser::MediaParser> parser;

  std::unique_ptr<Captions::CaptionCache> captionCache{};

  Audio::AudioData audioData{};

  uint32_t audioDurationRemaining = 0;

  uint32_t previousFrameIndex = 0;

  std::thread captionsThread;
  std::mutex captionsMutex;
  std::condition_variable captionsCV;
  bool captionsLoaded = false;
  bool captionsReady = false;

  bool fullscreen = false;

  bool shouldRecreateWindow = true;

  struct ShowControls {
    bool media = true;
    bool sfx = true;
  } showControls;

  struct SFX {
    bool grayscale = false;
  } sfx;

  void toggleFullscreen();

  void createWindow();

  void startCaptionsLoading();

  bool areCaptionsLoaded();

  void loadCaptions();

  void update();

  void handleKeyInput();

  void displayGui();

  void menuBarGui();

  void timelineGui();

  void volumeGui() const;

  void sfxGui();

  void navigateFrames(int numFrames) const;

  void loadNewFile();
};

#endif //MEDIAPLAYER_H
