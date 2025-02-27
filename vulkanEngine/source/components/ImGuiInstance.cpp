#include "ImGuiInstance.h"
#include "../components/Instance.h"
#include "../components/LogicalDevice.h"
#include "../components/PhysicalDevice.h"
#include "../components/Window.h"
#include "../pipelines/RenderPass.h"
#include "../pipelines/custom/GuiPipeline.h"
#include "../utilities/Buffers.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace VkEngine {
  ImGuiInstance::ImGuiInstance(const VkCommandPool& commandPool, const std::shared_ptr<Window>& window,
                               const std::shared_ptr<Instance>& instance,
                               const std::shared_ptr<PhysicalDevice>& physicalDevice,
                               const std::shared_ptr<LogicalDevice>& logicalDevice,
                               const std::shared_ptr<RenderPass>& renderPass,
                               const std::unique_ptr<GuiPipeline>& guiPipeline,
                               const bool useDockSpace)
    : useDockSpace(useDockSpace)
  {
    ImGui::CreateContext();

    window->initImGui();

    const SwapChainSupportDetails swapChainSupport = physicalDevice->getSwapChainSupport();

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    ImGui_ImplVulkan_InitInfo initInfo {
      .Instance = instance->getInstance(),
      .PhysicalDevice = physicalDevice->getPhysicalDevice(),
      .Device = logicalDevice->getDevice(),
      .Queue = logicalDevice->getGraphicsQueue(),
      .DescriptorPool = guiPipeline->getPool(),
      .RenderPass = renderPass->getRenderPass(),
      .MinImageCount = imageCount,
      .ImageCount = imageCount,
      .MSAASamples = physicalDevice->getMsaaSamples()
    };

    ImGui_ImplVulkan_Init(&initInfo);

    if (useDockSpace)
    {
      ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    const VkCommandBuffer commandBuffer = Buffers::beginSingleTimeCommands(logicalDevice, commandPool);
    ImGui_ImplVulkan_CreateFontsTexture();
    Buffers::endSingleTimeCommands(logicalDevice, commandPool, logicalDevice->getGraphicsQueue(), commandBuffer);

    createNewFrame();
  }

  ImGuiInstance::~ImGuiInstance()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiInstance::createNewFrame()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!useDockSpace)
    {
      return;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGuiWindowFlags dockspace_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("WindowDockSpace", nullptr, dockspace_flags)) {
      ImGui::PopStyleVar(3);

      const ImGuiID dockspaceID = ImGui::GetID("WindowDockSpace");
      ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

      if (dockNeedsUpdate) {
        // Rebuild layout
        ImGui::DockBuilderRemoveNode(dockspaceID);  // Only reset if update is needed
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());

        mainDock = dockspaceID;

        // Split nodes using stored percentages
        ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Left, leftDockPercent, &leftDock, &mainDock);
        ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Right, rightDockPercent, &rightDock, &mainDock);
        ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Up, topDockPercent, &topDock, &mainDock);
        ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Down, bottomDockPercent, &bottomDock, &mainDock);

        centerDock = mainDock;

        ImGui::DockBuilderFinish(dockspaceID);
        dockNeedsUpdate = false;
      }
    }
    ImGui::End();


    // ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    // ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    // ImGui::SetNextWindowBgAlpha(1.0f);
    //
    // ImGuiID id = ImGui::GetID("WindowDockSpace");
    // ImGui::DockBuilderRemoveNode(id); // Clear previous layout if any
    // ImGui::DockBuilderAddNode(id);    // Create new dock node
    //
    // if (ImGui::Begin("WindowDockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    // {
    //   const ImGuiID dockspaceID = ImGui::GetID("WindowDockSpace");
    //   ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    //
    //   if (dockNeedsUpdate)
    //   {
    //     // Rebuild the dock layout with current percentages
    //     ImGui::DockBuilderRemoveNode(dockspaceID);
    //     ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
    //     ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetWindowSize());
    //
    //     mainDock = dockspaceID;
    //
    //     // Split nodes using current percentages
    //     ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Left, leftDockPercent, &leftDock, &mainDock);
    //     ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Right, rightDockPercent, &rightDock, &mainDock);
    //     ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Up, topDockPercent, &topDock, &mainDock);
    //     ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Down, bottomDockPercent, &bottomDock, &mainDock);
    //
    //     centerDock = mainDock;
    //
    //     ImGui::DockBuilderFinish(dockspaceID);
    //     dockNeedsUpdate = false;
    //   }
    // }
    // ImGui::End();
  }

  void ImGuiInstance::dockTop(const char* widget) const
  {
    if (!mainDock)
    {
      return;
    }

    ImGui::DockBuilderDockWindow(widget, topDock);
  }

  void ImGuiInstance::dockBottom(const char* widget) const
  {
    if (!mainDock)
    {
      return;
    }

    ImGui::DockBuilderDockWindow(widget, bottomDock);
  }

  void ImGuiInstance::dockLeft(const char* widget) const
  {
    if (!mainDock)
    {
      return;
    }

    ImGui::DockBuilderDockWindow(widget, leftDock);
  }

  void ImGuiInstance::dockRight(const char* widget) const
  {
    if (!mainDock)
    {
      return;
    }

    ImGui::DockBuilderDockWindow(widget, rightDock);
  }

  void ImGuiInstance::dockCenter(const char* widget) const
  {
    if (!mainDock)
    {
      return;
    }

    ImGui::DockBuilderDockWindow(widget, centerDock);
  }

  void ImGuiInstance::setTopDockPercent(const float percent)
  {
    if (topDockPercent == percent)
    {
      return;
    }

    topDockPercent = percent;

    dockNeedsUpdate = true;
  }

  void ImGuiInstance::setBottomDockPercent(const float percent)
  {
    if (bottomDockPercent == percent)
    {
      return;
    }

    bottomDockPercent = percent;

    dockNeedsUpdate = true;
  }

  void ImGuiInstance::setLeftDockPercent(const float percent)
  {
    if (leftDockPercent == percent)
    {
      return;
    }

    leftDockPercent = percent;

    dockNeedsUpdate = true;
  }

  void ImGuiInstance::setRightDockPercent(const float percent)
  {
    if (rightDockPercent == percent)
    {
      return;
    }

    rightDockPercent = percent;

    dockNeedsUpdate = true;
  }
} // VkEngine