#include "MediaPlayer.h"
#include "../AudioToTxt/tests/test_whisper/audioDecoding.h"
#include <components/ImGuiInstance.h>
#include <iostream>
#include <filesystem>

MediaPlayer::MediaPlayer(const char* asset)
  : asset{asset}, vulkanEngine{std::make_unique<VkEngine::VulkanEngine>(vulkanEngineOptions)},
    parser{std::make_unique<AVParser::MediaParser>(asset)}
{
  loadCaptions();

  Audio::initSDL();
  Audio::convertWav(asset, "audio");

  ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());

  captionCache = std::make_unique<Captions::CaptionCache>("assets/subtitles.srt");
}

MediaPlayer::~MediaPlayer()
{
  Audio::deleteStream(audioData.stream);
  Audio::quitSDL();
}

void MediaPlayer::run()
{
  audioData = Audio::playAudio("audio.wav");
  audioDurationRemaining = audioData.duration;

  const auto frame = parser->getCurrentFrame();
  vulkanEngine->loadVideoFrame(frame.videoData, frame.frameWidth, frame.frameHeight);

  while (vulkanEngine->isActive())
  {
    update();
  }
}

void MediaPlayer::loadCaptions() const
{
  const std::string assetsPath = "assets/";

  const std::string audioFile = assetsPath + "audio.pcm";
  const std::string subtitleFile = assetsPath + "subtitles.srt";
  const std::string outputVideo = assetsPath + "output_with_subtitles_turbo.mp4";
  const std::string modelPath = "models/ggml-large-v3-turbo-q5_0.bin"; // "ggml-base.bin"

  if (!extractAudio(asset, audioFile))
  {
    throw std::runtime_error("Failed to generate formated audio file");
  }

  if (Captions::transcribeAudio(modelPath, audioFile, subtitleFile) != 0)
  {
    throw std::runtime_error("Failed to generate subtitles");
  }
}

void MediaPlayer::update()
{
  const auto gui = vulkanEngine->getImGuiInstance();

  handleKeyInput();

  gui->dockBottom("Media Player Controls");
  gui->setBottomDockPercent(0.3);
  displayGui();

  parser->update();

  const std::string captionFromCache = captionCache->getCaptionAtFrame(parser->getCurrentFrameIndex() / parser->getFrameRate() * 100);

  vulkanEngine->loadCaption(captionFromCache.c_str());

  if (const uint32_t currentFrameIndex = parser->getCurrentFrameIndex(); currentFrameIndex != previousFrameIndex)
  {
    const auto frame = parser->getCurrentFrame();

    vulkanEngine->loadVideoFrame(frame.videoData, frame.frameWidth, frame.frameHeight);

    previousFrameIndex = currentFrameIndex;
  }

  vulkanEngine->render();

  if (audioDurationRemaining > 0)
  {
    Audio::delay(1); // checks for extra input every 1 ms

    audioDurationRemaining -= 1;
  }
}

void MediaPlayer::handleKeyInput()
{
  // Struct to track key state for each key
  struct KeyState {
    bool wasPressed = false;
    int holdCounter = 0;
  };

  // Static map to track all key states
  static std::unordered_map<int, KeyState> keyStates;

  const uint32_t currentFrameIndex = parser->getCurrentFrameIndex();

  // Helper functions
  auto processKeyPress = [&](const int key, auto&& action)
  {
    const bool isPressed = vulkanEngine->keyIsPressed(key);
    auto&[wasPressed, holdCounter] = keyStates[key];

    if (isPressed && !wasPressed)
    {
      // Key was just pressed
      action(true, false, holdCounter);
    }
    else if (isPressed)
    {
      // Key is being held down
      holdCounter++;
      action(false, true, holdCounter);
    }
    else if (!isPressed && wasPressed)
    {
      // Key was just released
      action(false, false, holdCounter);
      holdCounter = 0;
    }

    wasPressed = isPressed;
  };

  // Handle play/pause toggle (Space)
  processKeyPress(GLFW_KEY_SPACE, [&](bool justPressed, bool held, int counter)
  {
    if (justPressed)
    {
      if (parser->getState() == AVParser::MediaState::PAUSED)
      {
        parser->play();
      }
      else if (parser->getState() == AVParser::MediaState::AUTO_PLAYING)
      {
        parser->pause();
      }
    }
  });

  // Helper function for frame navigation keys with common behavior
  auto handleNavKey = [&](const int key, const int initialJump, const int holdJump)
  {
    processKeyPress(key, [&](const bool justPressed, const bool held, const int counter)
    {
      if (justPressed)
      {
        navigateFrames(initialJump);
      }
      else if (held && counter % 10 == 0)
      {
        if (parser->getState() != AVParser::MediaState::MANUAL)
        {
          parser->pause();
        }
        navigateFrames(holdJump);
      }
      else if (!held && !justPressed && counter > 0)
      {
        // Just released after being held
        if (parser->getState() != AVParser::MediaState::MANUAL)
        {
          parser->play();
        }
      }
    });
  };

  // Handle right key (forward)
  handleNavKey(GLFW_KEY_RIGHT, 10, 5);

  // Handle left key (backward)
  handleNavKey(GLFW_KEY_LEFT, -10, -5);

  // Handle reset to beginning (R key)
  processKeyPress(GLFW_KEY_R, [&](const bool justPressed, bool held, int counter)
  {
    if (justPressed)
    {
      parser->loadFrameAt(0);
    }
  });
}

void MediaPlayer::displayGui()
{
  menuBarGui();

  ImGui::Begin("Media Player Controls");

  timelineGui();

  ImGui::Separator();

  volumeGui();

  ImGui::End();
}

void MediaPlayer::menuBarGui()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open Media", "Ctrl+O"))
      {
        // TODO: Open Video
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {
      if (ImGui::MenuItem("Controls"))
      {
        // TODO: Open Controls Dialogue
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void MediaPlayer::timelineGui()
{
  const float windowWidth = ImGui::GetContentRegionAvail().x;
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));

  // Media player time information
  uint32_t currentFrameIndex = parser->getCurrentFrameIndex();
  const uint32_t totalFrames = parser->getTotalFrames();

  // Center the seek bar
  ImGui::SetCursorPosX((windowWidth - ImGui::CalcItemWidth()) * 0.5f);

  // Seek bar (progress bar)
  if (ImGui::SliderInt("##timeline", reinterpret_cast<int*>(&currentFrameIndex), 0, static_cast<int>(totalFrames),""))
  {
    parser->loadFrameAt(currentFrameIndex);
  }

  // Transport control buttons
  ImGui::Separator();

  constexpr float buttonSize = 100.0f; // Adjusted button size to fit text
  constexpr float smallButtonSize = 50.0f; // Adjusted small button size to fit text

  // Calculate the total width of all controls
  constexpr float totalControlsWidth = buttonSize + smallButtonSize * 2; // 1 main button (Play/Pause) + 2 small buttons

  // Center the controls in the window
  const float centerPos = (windowWidth - totalControlsWidth) / 2.0f;
  ImGui::SetCursorPosX(centerPos);

  // Rewind button
  if (ImGui::Button("<<", ImVec2(smallButtonSize, 0)))
  {
    navigateFrames(-30);
  }
  ImGui::SameLine();

  // Play/Pause button
  if (parser->getState() == AVParser::MediaState::AUTO_PLAYING)
  {
    if (ImGui::Button("Pause", ImVec2(buttonSize, 0)))
    {
      parser->pause();
    }
  }
  else
  {
    if (ImGui::Button("Play", ImVec2(buttonSize, 0)))
    {
      parser->play();
    }
  }
  ImGui::SameLine();

  // Fast Forward button
  if (ImGui::Button(">>", ImVec2(smallButtonSize, 0)))
  {
    navigateFrames(30);
  }
}

void MediaPlayer::volumeGui()
{
  constexpr float buttonSize = 100.0f;

  // Volume control
  static float volume = 1.0f;
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Volume:");
  ImGui::SameLine();
  if (ImGui::Button(volume <= 0.01f ? "Mute" : "Unmute", ImVec2(buttonSize, 0)))
  {
    // Toggle mute
    volume = volume <= 0.01f ? 1.0f : 0.0f;
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(150);
  ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f, "%.2f");
  ImGui::PopItemWidth();
  ImGui::PopStyleVar();

  Audio::changeVolume(audioData.stream, volume);
}

void MediaPlayer::navigateFrames(const int numFrames) const
{
  const int32_t maxFrames = static_cast<int32_t>(parser->getTotalFrames());
  const int32_t currentFrame = static_cast<int32_t>(parser->getCurrentFrameIndex());

  const uint32_t newFrame = std::clamp(currentFrame + numFrames, 0, maxFrames);

  parser->loadFrameAt(newFrame);
}
