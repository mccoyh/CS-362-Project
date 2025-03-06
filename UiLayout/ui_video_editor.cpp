#include "ui_video_editor.h"
#include "config.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

UIVideoEditor::UIVideoEditor() : 
    currentVideoIndex(-1), 
    currentEditMode(EditMode::Trim),
    formatContext(nullptr),
    codecContext(nullptr),
    swsContext(nullptr) {
}

UIVideoEditor::~UIVideoEditor() {
    // Cleanup FFmpeg resources
    if (formatContext) avformat_close_input(&formatContext);
    if (codecContext) avcodec_free_context(&codecContext);
    if (swsContext) sws_freeContext(swsContext);
}

void UIVideoEditor::render() {
    ImGui::Begin("Video Editor");

    // Video selection
    if (!editQueue.empty()) {
        // Combo box for selecting video
        std::vector<const char*> videoNames;
        for (const auto& video : editQueue) {
            videoNames.push_back(video.filename.c_str());
        }

        // Video selection dropdown
        if (ImGui::Combo("Select Video", &currentVideoIndex, 
                         videoNames.data(), videoNames.size())) {
            // Video selected
        }

        // Edit mode selection
        renderEditModeSelector();

        // Render specific edit controls based on mode
        if (currentVideoIndex >= 0 && currentVideoIndex < editQueue.size()) {
            auto& currentVideo = editQueue[currentVideoIndex];

            switch (currentEditMode) {
                case EditMode::Trim:
                    renderTrimControls();
                    break;
                case EditMode::Crop:
                    renderCropControls();
                    break;
                case EditMode::Rotate:
                    renderRotationControls();
                    break;
                case EditMode::AdjustColor:
                    renderColorAdjustmentControls();
                    break;
            }

            // Export button
            if (ImGui::Button("Export Edited Video")) {
                std::string outputPath = VideoFileUtils::generateUniqueFilename(
                    currentVideo.filepath
                );
                if (exportEditedVideo(outputPath)) {
                    ImGui::OpenPopup("Export Successful");
                } else {
                    ImGui::OpenPopup("Export Failed");
                }
            }
        }
    } else {
        ImGui::Text("No videos in edit queue. Add a video to start editing.");
    }

    // Export result popups
    if (ImGui::BeginPopupModal("Export Successful")) {
        ImGui::Text("Video successfully exported!");
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Export Failed")) {
        ImGui::Text("Failed to export video. Please try again.");
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void UIVideoEditor::renderEditModeSelector() {
    ImGui::Text("Edit Mode:");
    int editMode = static_cast<int>(currentEditMode);
    
    ImGui::RadioButton("Trim", &editMode, static_cast<int>(EditMode::Trim)); 
    ImGui::SameLine();
    ImGui::RadioButton("Crop", &editMode, static_cast<int>(EditMode::Crop)); 
    ImGui::SameLine();
    ImGui::RadioButton("Rotate", &editMode, static_cast<int>(EditMode::Rotate)); 
    ImGui::SameLine();
    ImGui::RadioButton("Color", &editMode, static_cast<int>(EditMode::AdjustColor));

    currentEditMode = static_cast<EditMode>(editMode);
}

void UIVideoEditor::renderTrimControls() {
    auto& currentVideo = editQueue[currentVideoIndex];

    // Trim start and end sliders
    ImGui::SliderScalar("Trim Start", ImGuiDataType_Double, 
                        &currentVideo.trimStart, 
                        0.0, currentVideo.duration);
    ImGui::SliderScalar("Trim End", ImGuiDataType_Double, 
                        &currentVideo.trimEnd, 
                        0.0, currentVideo.duration);

    // Ensure trim end is not before trim start
    if (currentVideo.trimEnd < currentVideo.trimStart) {
        currentVideo.trimEnd = currentVideo.trimStart;
    }

    // Preview trimmed duration
    double trimmedDuration = currentVideo.trimEnd - currentVideo.trimStart;
    ImGui::Text("Trimmed Duration: %.2f seconds", trimmedDuration);
}

void UIVideoEditor::renderCropControls() {
    auto& currentVideo = editQueue[currentVideoIndex];

    static int cropWidth = currentVideo.width;
    static int cropHeight = currentVideo.height;
    static int cropX = 0;
    static int cropY = 0;

    ImGui::Text("Crop Settings:");
    ImGui::SliderInt("Width", &cropWidth, 1, currentVideo.width);
    ImGui::SliderInt("Height", &cropHeight, 1, currentVideo.height);
    ImGui::SliderInt("X Offset", &cropX, 0, currentVideo.width - cropWidth);
    ImGui::SliderInt("Y Offset", &cropY, 0, currentVideo.height - cropHeight);

    ImGui::Text("Crop Preview: %d x %d at (%d, %d)", 
                cropWidth, cropHeight, cropX, cropY);
}

void UIVideoEditor::renderRotationControls() {
    auto& currentVideo = editQueue[currentVideoIndex];

    ImGui::Text("Rotate Video:");
    
    ImGui::RadioButton("0°", &currentVideo.rotationAngle, 0); 
    ImGui::SameLine();
    ImGui::RadioButton("90°", &currentVideo.rotationAngle, 90); 
    ImGui::SameLine();
    ImGui::RadioButton("180°", &currentVideo.rotationAngle, 180); 
    ImGui::SameLine();
    ImGui::RadioButton("270°", &currentVideo.rotationAngle, 270);

    ImGui::Text("Current Rotation: %d°", currentVideo.rotationAngle);
}

void UIVideoEditor::renderColorAdjustmentControls() {
    auto& currentVideo = editQueue[currentVideoIndex];

    // Brightness, contrast, and saturation sliders
    ImGui::SliderFloat("Brightness", &currentVideo.brightness, -1.0f, 1.0f);
    ImGui::SliderFloat("Contrast", &currentVideo.contrast, 0.0f, 2.0f);
    ImGui::SliderFloat("Saturation", &currentVideo.saturation, 0.0f, 2.0f);

    // Reset button
    if (ImGui::Button("Reset Adjustments")) {
        resetVideoParameters(currentVideo);
    }
}

void UIVideoEditor::addVideoToEditQueue(const std::string& filepath) {
    // Validate video file
    if (!VideoFileUtils::isValidVideoFile(filepath)) {
        std::cerr << "Invalid video file: " << filepath << std::endl;
        return;
    }

    EditableVideo newVideo;
    newVideo.filepath = filepath;
    newVideo.filename = std::filesystem::path(filepath).filename().string();

    // Load video metadata
    if (loadVideoMetadata(newVideo)) {
        // Set default trim to full video duration
        newVideo.trimStart = 0.0;
        newVideo.trimEnd = newVideo.duration;
        newVideo.rotationAngle = 0;
        newVideo.brightness = 0.0f;
        newVideo.contrast = 1.0f;
        newVideo.saturation = 1.0f;

        editQueue.push_back(newVideo);
    }
}

void UIVideoEditor::removeVideoFromEditQueue(int index) {
    if (index >= 0 && index < editQueue.size()) {
        editQueue.erase(editQueue.begin() + index);
    }
}

void UIVideoEditor::clearEditQueue() {
    editQueue.clear();
    currentVideoIndex = -1;
}

void UIVideoEditor::trimVideo(double start, double end) {
    if (currentVideoIndex < 0 || currentVideoIndex >= editQueue.size()) return;

    auto& video = editQueue[currentVideoIndex];
    video.trimStart = std::max(0.0, start);
    video.trimEnd = std::min(end, video.duration);
}

void UIVideoEditor::rotateVideo(int angle) {
    if (currentVideoIndex < 0 || currentVideoIndex >= editQueue.size()) return;

    auto& video = editQueue[currentVideoIndex];
    video.rotationAngle = angle;
}

void UIVideoEditor::cropVideo(int x, int y, int width, int height) {
    if (currentVideoIndex < 0 || currentVideoIndex >= editQueue.size()) return;

    auto& video = editQueue[currentVideoIndex];
    // TODO: Implement actual video cropping logic
    std::cout << "Cropping video: " 
              << width << "x" << height 
              << " at (" << x << ", " << y << ")" << std::endl;
}

void UIVideoEditor::adjustColor(float brightness, float contrast, float saturation) {
    if (currentVideoIndex < 0 || currentVideoIndex >= editQueue.size()) return;

    auto& video = editQueue[currentVideoIndex];
    video.brightness = std::clamp(brightness, -1.0f, 1.0f);
    video.contrast = std::clamp(contrast, 0.0f, 2.0f);
    video.saturation = std::clamp(saturation, 0.0f, 2.0f);
}

bool UIVideoEditor::exportEditedVideo(const std::string& outputPath) {
    if (currentVideoIndex < 0 || currentVideoIndex >= editQueue.size()) {
        return false;
    }

    const auto& video = editQueue[currentVideoIndex];

    try {
        // Actual video processing
        return processVideoEdit(video, outputPath);
    } catch (const std::exception& e) {
        std::cerr << "Export failed: " << e.what() << std::endl;
        return false;
    }
}

bool UIVideoEditor::loadVideoMetadata(EditableVideo& video) {
    return VideoFileUtils::getVideoMetadata(
        video.filepath, 
        video.width, 
        video.height, 
        video.duration
    );
}

bool UIVideoEditor::processVideoEdit(const EditableVideo& video, const std::string& outputPath) {
    // Placeholder for actual video processing
    // In a real implementation, this would use FFmpeg for video editing
    std::cout << "Processing video: " << video.filename << std::endl;
    std::cout << "Trim: " << video.trimStart << " - " << video.trimEnd << std::endl;
    std::cout << "Rotation: " << video.rotationAngle << std::endl;
    std::cout << "Color Adjustments:" << std::endl;
    std::cout << "  Brightness: " << video.brightness << std::endl;
    std::cout << "  Contrast: " << video.contrast << std::endl;
    std::cout << "  Saturation: " << video.saturation << std::endl;
    std::cout << "Output Path: " << outputPath << std::endl;

    // Simulate processing time
    // std::this_thread::sleep_for(std::chrono::seconds(2));

    return true;
}

void UIVideoEditor::resetVideoParameters(EditableVideo& video) {
    video.trimStart = 0.0;
    video.trimEnd = video.duration;
    video.rotationAngle = 0;
    video.brightness = 0.0f;
    video.contrast = 1.0f;
    video.saturation = 1.0f;
}

bool UIVideoEditor::isVideoSelected() const {
    return currentVideoIndex >= 0 && currentVideoIndex
    
