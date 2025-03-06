#include <source/AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <GLFW/glfw3.h>

void displayControls(AVParser::MediaParser& parser);

int main(const int argc, char* argv[])
{
  try
  {
    auto parser = AVParser::MediaParser(argc == 2 ? argv[1] : "assets/sample_720.mp4");

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

    // For debouncing key presses
    bool spaceWasPressed = false;
    bool rightWasPressed = false;
    bool leftWasPressed = false;
    bool rWasPressed = false;
    bool mWasPressed = false;
    uint32_t currentframe = parser.getCurrentFrameIndex
    vulkanEngine.loadCaption("Press SPACE to pause/resume, R to restart, LEFT/RIGHT to navigate.");

    while (vulkanEngine.isActive())
    {
      // Handle keyboard input using keyIsPressed
      bool spaceIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_SPACE);
      if (spaceIsPressed && !spaceWasPressed) {
        if (parser.getState() == AVParser::MediaState::PAUSED  ) {
          parser.play();
          std::cout << "Video resumed" << std::endl;
        } else if (parser.getState() == AVParser::MediaState::AUTO_PLAYING) {
          parser.pause();
          std::cout << "Video paused" << std::endl;
        }
      }
      spaceWasPressed = spaceIsPressed;

      bool rightIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_RIGHT);
      if (rightIsPressed) {
        if (!rightWasPressed) {
          // Key was just pressed (first frame)
          parser.loadFrameAt(currentframe+10);
          std::cout << "Right arrow pressed - Next frame" << std::endl;
        } else {
          // Key is being held down
          static int holdCounter = 0;
          if (++holdCounter % 10 == 0) {  // Every 10 frames while holding
            if (parser.getState() != AVParser::MediaState::MANUAL) {
              parser.pause();
            }
            parser.loadFrameAt(currentframe+5);
            std::cout << "Right arrow held - Advancing frame" << std::endl;
          }
        }
      } else {
        // Reset counter when key is released
        if (rightWasPressed) {
          if (parser.getState() != AVParser::MediaState::MANUAL) {
            parser.play();
          }
          std::cout << "Right arrow released" << std::endl;
        }
      }
      rightWasPressed = rightIsPressed;

      bool leftIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_LEFT);
      if (leftIsPressed) {
        if (!leftWasPressed) {
          // Key was just pressed (first frame)
          parser.loadFrameAt(currentframe-5);
          std::cout << "Left arrow pressed - Previous frame" << std::endl;
        } else {
          // Key is being held down
          static int holdCounter = 0;
          if (++holdCounter % 10 == 0) {  // Every 10 frames while holding
            if (parser.getState() != AVParser::MediaState::MANUAL) {
              parser.pause();
            }
            parser.loadFrameAt(currentframe-5);
            std::cout << "Left arrow held - Rewinding frame" << std::endl;
          }
        }
      } else {
        // Reset counter when key is released
        if (leftWasPressed) {
          if (parser.getState() != AVParser::MediaState::MANUAL) {
            parser.play();
          }
          std::cout << "Left arrow released" << std::endl;
        }
      }
      leftWasPressed = leftIsPressed;

      bool rIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_R);
      if (rIsPressed && !rWasPressed) {
        parser.loadFrameAt(0);
        std::cout << "Restarting video" << std::endl;
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

  // Media player time information
  uint32_t currentFrameIndex = parser.getCurrentFrameIndex();
  int totalFrames = parser.getTotalFrames();
  float currentTime = currentFrameIndex / 30.0f; // Assuming 30fps, adjust if needed
  float totalTime = totalFrames / 30.0f;




  // Seek bar (progress bar)

  if (ImGui::SliderInt("##timeline", &currentFrameIndex, 0, totalFrames, ""))
  {
    parser.loadFrameAt(currentFrameIndex);
  }

  // Transport control buttons
  ImGui::Separator();

  const float buttonSize = 75.0f;
  const float smallButtonSize = 40.0f;

  // Center the main playback controls
  float centerPos = (windowWidth - (buttonSize * 3 + smallButtonSize * 2)) / 2.0f;
  ImGui::SetCursorPosX(centerPos);

  // Rewind button (10 seconds)
  if (ImGui::Button("<<", ImVec2(smallButtonSize, 0)))
  {
    parser.loadFrameAt(currentFrameIndex-30)
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
    parser.loadFrameAt(currentFrameIndex+30)
  }

  ImGui::Separator();

  // Bottom row with additional controls
  // Volume control
  static float volume = 1.0f;
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Volume:");
  ImGui::SameLine();
  if (ImGui::Button(volume <= 0.01f ? "Mute" : "Unmute", ImVec2(50, 0)))
  {
    // Toggle mute
    volume = (volume <= 0.01f) ? 1.0f : 0.0f;
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(150);
  ImGui::SliderFloat("##volume", &volume, 0.0f, 1.0f, "%.2f");
  ImGui::PopItemWidth();

  // Manual mode toggle on the right
  ImGui::SameLine();
  ImGui::SetCursorPosX(windowWidth - 120);
  
  

  ImGui::PopStyleVar();
  ImGui::End();
}