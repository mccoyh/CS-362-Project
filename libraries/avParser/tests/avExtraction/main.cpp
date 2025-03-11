#include <AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>

void displayControls(AVParser::MediaParser& parser);

constexpr AVParser::AudioParams audioParams;

int main(const int argc, char* argv[])
{
  try
  {
    auto parser = AVParser::MediaParser(argc == 2 ? argv[1] : "assets/sample_720.mp4", audioParams);

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

    while (vulkanEngine.isActive())
    {
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
  // Add the "Change Video" button
  if (ImGui::Button("Change Video"))
  {
    try {
      parser.setFilepath("assets/Valorant_test.mp4");  
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;  
    }
  }

  ImGui::End();
}