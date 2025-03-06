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

    while (vulkanEngine.isActive())
    {
      // Handle keyboard input using keyIsPressed
      bool spaceIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_SPACE);
      if (spaceIsPressed && !spaceWasPressed) {
        if (parser.getState() == AVParser::MediaState::PAUSED) {
          parser.play();
          std::cout << "Video resumed" << std::endl;
        } else if (parser.getState() == AVParser::MediaState::AUTO_PLAYING) {
          parser.pause();
          std::cout << "Video paused" << std::endl;
        }
      }
      spaceWasPressed = spaceIsPressed;

      bool rightIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_RIGHT);
      if (rightIsPressed && !rightWasPressed) {
        parser.loadNextFrame();
        std::cout << "Next frame" << std::endl;
      }
      rightWasPressed = rightIsPressed;

      bool leftIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_LEFT);
      if (leftIsPressed && !leftWasPressed) {
        parser.loadPreviousFrame();
        std::cout << "Previous frame" << std::endl;
      }
      leftWasPressed = leftIsPressed;

      bool rIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_R);
      if (rIsPressed && !rWasPressed) {
        parser.loadFrameAt(0);
        std::cout << "Restarting video" << std::endl;
      }
      rWasPressed = rIsPressed;

      // Toggle between manual and automatic modes
      bool mIsPressed = vulkanEngine.keyIsPressed(GLFW_KEY_M);
      if (mIsPressed && !mWasPressed) {
        bool isManual = parser.getState() == AVParser::MediaState::MANUAL;
        parser.setManual(!isManual);
        std::cout << "Toggled to " << (!isManual ? "manual" : "automatic") << " mode" << std::endl;
      }
      mWasPressed = mIsPressed;

      gui->dockBottom("AV Controls");

      gui->setBottomDockPercent(0.2);

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
  ImGui::Begin("AV Controls");

  int currentFrameIndex = static_cast<int>(parser.getCurrentFrameIndex());

  switch (parser.getState())
  {
    case AVParser::MediaState::AUTO_PLAYING:
      if (ImGui::Button("MANUAL"))
      {
        parser.setManual(true);
      }
      ImGui::SameLine();
      if (ImGui::Button("Pause"))
      {
        parser.pause();
      }
      break;
    case AVParser::MediaState::PAUSED:
      if (ImGui::Button("MANUAL"))
      {
        parser.setManual(true);
      }
      ImGui::SameLine();
      if (ImGui::Button("Play"))
      {
        parser.play();
      }
      break;
    case AVParser::MediaState::MANUAL:
      if (ImGui::Button("AUTOMATIC"))
      {
        parser.setManual(false);
      }

      ImGui::SameLine();
      ImGui::Button("BACKWARD");
      if (ImGui::IsItemActive())
      {
        parser.loadPreviousFrame();
      }
      ImGui::SameLine();
      ImGui::Button("FORWARD");
      if (ImGui::IsItemActive())
      {
        parser.loadNextFrame();
      }

      if (ImGui::SliderInt("Frames", &currentFrameIndex, 0, parser.getTotalFrames()))
      {
        parser.loadFrameAt(currentFrameIndex);
      }


    default: break;
  }

  ImGui::End();
}