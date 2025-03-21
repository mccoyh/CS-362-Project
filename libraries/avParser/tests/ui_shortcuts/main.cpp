#include <AVParser.h>
#include <VulkanEngine.h>
#include <components/ImGuiInstance.h>
#include <iostream>
#include <GLFW/glfw3.h>

void displayControls(AVParser::MediaParser& parser);
void navigateFrames(AVParser::MediaParser& parser, uint32_t, int);

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

    // For debouncing key presses
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