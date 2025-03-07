#include <audio.h>
#include <AudioToTxt.h>
#include <AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <filesystem>
#include "../AudioToTxt/tests/test_whisper/audioDecoding.h"

constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
  .WINDOW_WIDTH = 1000,
  .WINDOW_HEIGHT = 600,
  .WINDOW_TITLE = "Medos Media Player"
};

void displayControls(AVParser::MediaParser& parser);
void navigateFrames(AVParser::MediaParser& parser, uint32_t, int);

void handleKeyInput(AVParser::MediaParser& parser, const VkEngine::VulkanEngine& vulkanEngine);

void loadCaptions(const char* asset)
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

int main(const int argc, char* argv[])
{
  try
  {
    const auto asset = argc == 2 ? argv[1] : "assets/CS_test.mp4";

    // Initialize Captions
    loadCaptions(asset);
    Captions::CaptionCache cache("assets/subtitles.srt");

    // Initialize Audio
    Audio::initSDL();

    Audio::convertWav(asset, "audio");

    const Audio::AudioData audio = Audio::playAudio("audio.wav");
    uint32_t duration = audio.duration;

    // Initialize Graphics
    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());
    const auto gui = vulkanEngine.getImGuiInstance();

    // Initialize Media Parser
    auto parser = AVParser::MediaParser(asset);

    while (vulkanEngine.isActive())
    {
      handleKeyInput(parser, vulkanEngine);

      gui->dockBottom("Media Player Controls");
      gui->setBottomDockPercent(0.3);
      displayControls(parser);

      parser.update();

      const std::string captionFromCache = cache.getCaptionAtFrame(parser.getCurrentFrameIndex() / parser.getFrameRate() * 100);

      vulkanEngine.loadCaption(captionFromCache.c_str());

      const auto frame = parser.getCurrentFrame();

      vulkanEngine.loadVideoFrame(frame.videoData, frame.frameWidth, frame.frameHeight);

      vulkanEngine.render();

      if (duration > 0)
      {
        Audio::delay(1); // checks for extra input every 1 ms

        duration -= 1;
      }
    }

    Audio::deleteStream(audio.stream);
    Audio::quitSDL();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void displayControls(AVParser::MediaParser& parser)
{
  ImGui::Begin("Media Player Controls");

  const float windowWidth = ImGui::GetContentRegionAvail().x;
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 6));

  // Media player time information
  uint32_t currentFrameIndex = parser.getCurrentFrameIndex();
  const uint32_t totalFrames = parser.getTotalFrames();

  // Center the seek bar
  ImGui::SetCursorPosX((windowWidth - ImGui::CalcItemWidth()) * 0.5f);

  // Seek bar (progress bar)
  if (ImGui::SliderInt("##timeline", reinterpret_cast<int*>(&currentFrameIndex), 0, static_cast<int>(totalFrames),""))
  {
    parser.loadFrameAt(currentFrameIndex);
  }

  // Transport control buttons
  ImGui::Separator();

  constexpr float buttonSize = 100.0f; // Adjusted button size to fit text
  constexpr float smallButtonSize = 50.0f; // Adjusted small button size to fit text

  // Center the main playback controls
  const float centerPos = (windowWidth - (buttonSize * 3 + smallButtonSize * 2)) / 2.0f;
  ImGui::SetCursorPosX(centerPos);

  // Rewind button (10 seconds)
  if (ImGui::Button("<<", ImVec2(smallButtonSize, 0)))
  {
    navigateFrames(parser, currentFrameIndex, -30);
  }
  ImGui::SameLine();

  // Play/Pause button
  if (parser.getState() == AVParser::MediaState::AUTO_PLAYING)
  {
    if (ImGui::Button("Pause", ImVec2(buttonSize, 0))) // Pause symbol
    {
      parser.pause();
    }
  }
  else
  {
    if (ImGui::Button("Play", ImVec2(buttonSize, 0))) // Play symbol
    {
      parser.play();
    }
  }
  ImGui::SameLine();

  // Fast Forward button (10 seconds)
  if (ImGui::Button(">>", ImVec2(smallButtonSize, 0)))
  {
    navigateFrames(parser, currentFrameIndex, 30);
  }
  ImGui::Separator();

  // Bottom row with additional controls
  // Volume control
  static float volume = 1.0f;
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Volume:");
  ImGui::SameLine();
  if (ImGui::Button(volume <= 0.01f ? "Mute" : "Unmute", ImVec2(buttonSize, 0)))
  {
    // Toggle mute
    volume = (volume <= 0.01f) ? 1.0f : 0.0f;
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(150);
  ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f, "%.2f");
  ImGui::PopItemWidth();
  ImGui::PopStyleVar();
  ImGui::End();
}

void navigateFrames(AVParser::MediaParser& parser, const uint32_t currentframe, int n){
  uint32_t maxframes = parser.getTotalFrames();
  if (n > 0) {
    parser.loadFrameAt((currentframe + n > maxframes) ? maxframes : (currentframe + n));
  } else {
    parser.loadFrameAt((static_cast<int>(currentframe) + n < 0) ? 0 : currentframe + n);
  }
}

void handleKeyInput(AVParser::MediaParser& parser, const VkEngine::VulkanEngine& vulkanEngine)
{
  // For debouncing key presses
  static bool spaceWasPressed = false;
  static bool rightWasPressed = false;
  static bool leftWasPressed = false;
  static bool rWasPressed = false;

  const uint32_t currentFrameIndex = parser.getCurrentFrameIndex();

  // Handle keyboard input using keyIsPressed
  const bool spaceIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_SPACE);
  if (spaceIsPressed && !spaceWasPressed) {
    if (parser.getState() == AVParser::MediaState::PAUSED) {
      parser.play();

    } else if (parser.getState() == AVParser::MediaState::AUTO_PLAYING) {
      parser.pause();

    }
  }
  spaceWasPressed = spaceIsPressed;

  const bool rightIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_RIGHT);
  if (rightIsPressed) {
    if (!rightWasPressed) {
      // Key was just pressed (first frame)
      navigateFrames(parser, currentFrameIndex, 10);

    } else {
      // Key is being held down
      static int holdCounter = 0;
      if (++holdCounter % 10 == 0) {  // Every 10 frames while holding
        if (parser.getState() != AVParser::MediaState::MANUAL) {
          parser.pause();
        }
        navigateFrames(parser, currentFrameIndex, 5);
      }
    }
  } else {
    // Reset counter when key is released
    if (rightWasPressed) {
      if (parser.getState() != AVParser::MediaState::MANUAL) {
        parser.play();
      }

    }
  }
  rightWasPressed = rightIsPressed;
  const bool leftIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_LEFT);
  if (leftIsPressed) {
    if (!leftWasPressed) {
      // Key was just pressed (first frame)
      navigateFrames(parser, currentFrameIndex, -10);

    } else {
      // Key is being held down
      static int holdCounter = 0;
      if (++holdCounter % 10 == 0) {  // Every 10 frames while holding
        if (parser.getState() != AVParser::MediaState::MANUAL) {
          parser.pause();
        }
        navigateFrames(parser, currentFrameIndex, -5);

      }
    }
  } else {
    // Reset counter when key is released
    if (leftWasPressed) {
      if (parser.getState() != AVParser::MediaState::MANUAL) {
        parser.play();
      }

    }
  }
  leftWasPressed = leftIsPressed;
  const bool rIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_R);
  if (rIsPressed && !rWasPressed) {
    parser.loadFrameAt(0);

  }
  rWasPressed = rIsPressed;
}
