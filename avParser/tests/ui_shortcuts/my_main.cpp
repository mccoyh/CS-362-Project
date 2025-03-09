#include <source/AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include "NativeFileDialog.h"  // Include the NativeFileDialog header

void displayControls(AVParser::MediaParser& parser);
void navigateFrames(AVParser::MediaParser& parser, uint32_t currentFrameIndex, int n);
std::string openFileDialog(); // Function to open the file dialog

int main(const int argc, char* argv[])
{
  try
  {
    // Open file dialog to choose a video file
    std::string videoPath = openFileDialog();
    if (videoPath.empty())
    {
      std::cerr << "No file selected, using default path." << std::endl;
      videoPath = "assets/sample_720.mp4"; // Fallback to default path if no file selected
    }

    auto parser = AVParser::MediaParser(videoPath.c_str());
    const auto frameData = parser.getCurrentFrame();

    std::cout << "Frame Width: " << frameData.frameWidth << "\n"
              << "Frame Height: " << frameData.frameHeight << "\n"
              << "Total Frames: " << parser.getTotalFrames() << std::endl;

    constexpr VkEngine::VulkanEngineOptions vulkanEngineOptions {
      .WINDOW_WIDTH = 600,
      .WINDOW_HEIGHT = 400,
      .WINDOW_TITLE = "Video Decoding"
    };

    auto vulkanEngine = VkEngine::VulkanEngine(vulkanEngineOptions);
    ImGui::SetCurrentContext(VkEngine::VulkanEngine::getImGuiContext());
    const auto gui = vulkanEngine.getImGuiInstance();

    bool spaceWasPressed = false;
    bool rightWasPressed = false;
    bool leftWasPressed = false;
    bool rWasPressed = false;
  
    vulkanEngine.loadCaption("Press SPACE to pause/resume, R to restart, LEFT/RIGHT to navigate.");

    while (vulkanEngine.isActive())
    {
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
          navigateFrames(parser, currentFrameIndex, 10); // Jump forward 10 frames
        } else {
          static int holdCounter = 0;
          if (++holdCounter % 10 == 0) {  // Every 10 frames while holding
            if (parser.getState() != AVParser::MediaState::MANUAL) {
              parser.pause();
            }
            navigateFrames(parser, currentFrameIndex, 5); // Jump forward 5 frames
          }
        }
      } else {
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
          navigateFrames(parser, currentFrameIndex, -10); // Jump backward 10 frames
        } else {
          static int holdCounter = 0;
          if (++holdCounter % 10 == 0) {
            if (parser.getState() != AVParser::MediaState::MANUAL) {
              parser.pause();
            }
            navigateFrames(parser, currentFrameIndex, -5); // Jump backward 5 frames
          }
        }
      } else {
        if (leftWasPressed) {
          if (parser.getState() != AVParser::MediaState::MANUAL) {
            parser.play();
          }
        }
      }
      leftWasPressed = leftIsPressed;

      const bool rIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_R);
      if (rIsPressed && !rWasPressed) {
        parser.loadFrameAt(0); // Restart the video
      }
      rWasPressed = rIsPressed;

      gui->dockBottom("Media Player Controls");
      gui->setBottomDockPercent(0.25);
      displayControls(parser);

      parser.update();

      const auto frame = parser.getCurrentFrame();
      vulkanEngine.loadVideoFrame(frame.videoData, frame.frameWidth, frame.frameHeight);

      vulkanEngine.render();
    }
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

  uint32_t currentFrameIndex = parser.getCurrentFrameIndex();
  const uint32_t totalFrames = parser.getTotalFrames();
  ImGui::SetCursorPosX((windowWidth - ImGui::CalcItemWidth()) * 0.5f);

  // Seek bar (progress bar)
  if (ImGui::SliderInt("##timeline", reinterpret_cast<int*>(&currentFrameIndex), 0, static_cast<int>(totalFrames),""))
  {
    parser.loadFrameAt(currentFrameIndex);
  }

  // Transport control buttons
  ImGui::Separator();
  constexpr float buttonSize = 100.0f;
  constexpr float smallButtonSize = 50.0f;

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
    if (ImGui::Button("Pause", ImVec2(buttonSize, 0)))
    {
      parser.pause();
    }
  }
  else
  {
    if (ImGui::Button("Play", ImVec2(buttonSize, 0)))
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

  // Volume control
  static float volume = 1.0f;
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Volume:");
  ImGui::SameLine();
  if (ImGui::Button(volume <= 0.01f ? "Mute" : "Unmute", ImVec2(buttonSize, 0))) 
  {
    volume = (volume <= 0.01f) ? 1.0f : 0.0f;
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(150);
  ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f, "%.2f");
  ImGui::PopItemWidth();
  
  ImGui::PopStyleVar();
  ImGui::End();
}

void navigateFrames(AVParser::MediaParser& parser, const uint32_t currentFrameIndex, int n)
{
  uint32_t maxFrames = parser.getTotalFrames();
  if (n > 0)
  {
    parser.loadFrameAt((currentFrameIndex + n > maxFrames) ? maxFrames : (currentFrameIndex + n));
  }
  else
  {
    parser.loadFrameAt((static_cast<int>(currentFrameIndex) + n < 0) ? 0 : currentFrameIndex + n);
  }
}

std::string openFileDialog()
{
  nfdfilteritem_t filters[] = {
    { "MP4 Video Files", "*.mp4" },
    { "AVI Video Files", "*.avi" },
    { "All Files", "*" }
  };

  nfdchar_t *outPath = nullptr;
  nfdresult_t result = NFD_OpenDialog(&outPath, filters, 3, nullptr);

  if (result == NFD_OKAY)
  {
    std::string filePath = outPath;
    NFD_FreePath(outPath); // Free the memory allocated by NFD
    return filePath;
  }
  else
  {
    std::cerr << "Error opening file: " << NFD_GetError() << std::endl;
    return "";
  }
}