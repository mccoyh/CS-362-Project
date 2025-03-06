#include "ui_main_window.h"
#include "config.h"

UIMainWindow::UIMainWindow() : 
    currentTab(ActiveTab::VideoPlayer) {
    applyTheme();
}

void UIMainWindow::applyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Dark mode styling
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
}

void UIMainWindow::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Video", "Ctrl+O")) {
                fileExplorer.openFileDialog();
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // TODO: Implement clean exit
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            const char* tabNames[] = {
                "Video Player", 
                "Playlist", 
                "Video Editor", 
                "Settings", 
                "File Explorer"
            };

            for (int i = 0; i < 5; ++i) {
                if (ImGui::MenuItem(tabNames[i])) {
                    currentTab = static_cast<ActiveTab>(i);
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIMainWindow::renderActiveTab() {
    switch (currentTab) {
        case ActiveTab::VideoPlayer:
            videoPlayer.render();
            break;
        case ActiveTab::Playlist:
            playlist.render();
            break;
        case ActiveTab::VideoEditor:
            videoEditor.render();
            break;
        case ActiveTab::Settings:
            settings.render();
            break;
        case ActiveTab::FileExplorer:
            fileExplorer.render();
            break;
    }
}

void UIMainWindow::render() {
    // Ensure fullscreen dockspace
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags windowFlags = 
        ImGuiWindowFlags_MenuBar | 
        ImGuiWindowFlags_NoDocking | 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin(MediaPlayerConfig::APP_NAME, nullptr, windowFlags);
    
    renderMenuBar();
    renderActiveTab();

    ImGui::End();
}
