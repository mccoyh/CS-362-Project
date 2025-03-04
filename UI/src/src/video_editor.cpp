#include "video_editor.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

VideoEditor::VideoEditor() : 
    currentOperation(EditOperation::None),
    isTrimActive(false),
    trimStartTime(0.0f),
    trimEndTime(0.0f),
    hasSelectedStartTime(false),
    hasSelectedEndTime(false),
    rotationAngle(0),
    isCropActive(false),
    cropRect(0.1f, 0.1f, 0.8f, 0.8f),  // Default to 10% margins
    isDraggingCrop(false),
    dragCorner(0),
    selectedClipIndex(-1) {
}

VideoEditor::~VideoEditor() {
}

void VideoEditor::renderEditor(FFmpegDecoder& decoder) {
    if (ImGui::Begin("Video Editor")) {
        // Operation selection
        const char* operations[] = { "None", "Trim", "Rotate", "Crop", "Stitch" };
        int currentOp = static_cast<int>(currentOperation);
        
        if (ImGui::Combo("Operation", &currentOp, operations, IM_ARRAYSIZE(operations))) {
            // Changed operation
            currentOperation = static_cast<EditOperation>(currentOp);
            
            // Reset states for all operations
            resetTrimState();
            resetRotateState();
            resetCropState();
            resetStitchState();
        }
        
        ImGui::Separator();
        
        // Render the appropriate controls based on the selected operation
        switch (currentOperation) {
            case EditOperation::Trim:
                renderTrimControls(decoder);
                break;
            case EditOperation::Rotate:
                renderRotateControls();
                break;
            case EditOperation::Crop:
                renderCropControls(decoder);
                break;
            case EditOperation::Stitch:
                renderStitchControls();
                break;
            default:
                ImGui::Text("Select an operation to begin editing.");
                break;
        }
        
        ImGui::Separator();
        
        // Apply and Cancel buttons
        if (currentOperation != EditOperation::None) {
            if (ImGui::Button("Apply Edits")) {
                applyEdits(decoder);
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Cancel Edits")) {
                resetEditorState();
            }
        }
    }
    ImGui::End();
}

void VideoEditor::renderTrimControls(FFmpegDecoder& decoder) {
    ImGui::Text("Trim Video");
    
    float duration = decoder.getDuration();
    float currentPos = decoder.getCurrentPosition();
    
    // Toggle for trim mode
    if (ImGui::Checkbox("Enable Trim", &isTrimActive)) {
        if (!isTrimActive) {
            resetTrimState();
        }
    }
    
    if (isTrimActive) {
        ImGui::Text("Current Position: %.2f s", currentPos);
        
        // Set trim start time
        if (ImGui::Button("Set Start Time")) {
            trimStartTime = currentPos;
            hasSelectedStartTime = true;
        }
        
        if (hasSelectedStartTime) {
            ImGui::SameLine();
            ImGui::Text("Start: %.2f s", trimStartTime);
        }
        
        // Set trim end time
        if (ImGui::Button("Set End Time")) {
            trimEndTime = currentPos;
            hasSelectedEndTime = true;
        }
        
        if (hasSelectedEndTime) {
            ImGui::SameLine();
            ImGui::Text("End: %.2f s", trimEndTime);
        }
        
        // Draw a timeline preview with trim markers
        ImGui::PushStyleColor(ImGui::Col_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGui::Col_SliderGrab, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::SliderFloat("##timeline", &currentPos, 0.0f, duration, "");
        ImGui::PopStyleColor(2);
        
        // Draw trim markers
        if (hasSelectedStartTime || hasSelectedEndTime) {
            ImGui::Text("Trim Preview:");
            ImGui::PushStyleColor(ImGui::Col_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            
            float previewStart = hasSelectedStartTime ? trimStartTime : 0.0f;
            float previewEnd = hasSelectedEndTime ? trimEndTime : duration;
            
            ImVec2 timeline_pos = ImGui::GetCursorScreenPos();
            ImVec2 timeline_size = ImVec2(ImGui::GetContentRegionAvail().x, 8.0f);
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            
            // Draw background
            draw_list->AddRectFilled(
                timeline_pos,
                ImVec2(timeline_pos.x + timeline_size.x, timeline_pos.y + timeline_size.y),
                ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f))
            );
            
            // Draw selected area
            float startX = timeline_pos.x + (previewStart / duration) * timeline_size.x;
            float endX = timeline_pos.x + (previewEnd / duration) * timeline_size.x;
            
            draw_list->AddRectFilled(
                ImVec2(startX, timeline_pos.y),
                ImVec2(endX, timeline_pos.y + timeline_size.y),
                ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.9f, 1.0f))
            );
            
            ImGui::Dummy(timeline_size);
            ImGui::PopStyleColor();
        }
    }
}

void VideoEditor::renderRotateControls() {
    ImGui::Text("Rotate Video");
    
    // Rotation angle selection
    const char* angles[] = { "0°", "90° Clockwise", "180°", "90° Counter-Clockwise" };
    int angleIndex = rotationAngle / 90;
    
    if (ImGui::Combo("Rotation Angle", &angleIndex, angles, IM_ARRAYSIZE(angles))) {
        rotationAngle = angleIndex * 90;
    }
    
    // Rotation preview
    ImGui::Text("Preview:");
    
    ImVec2 previewSize(200, 200);
    ImVec2 center = ImGui::GetCursorScreenPos();
    center.x += previewSize.x / 2;
    center.y += previewSize.y / 2;
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // Draw a simple rectangle to represent the video frame
    float halfWidth = 80.0f;
    float halfHeight = 60.0f;
    
    // Apply rotation transformation
    float angle = rotationAngle * 3.14159f / 180.0f;
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    
    ImVec2 p1, p2, p3, p4;
    
    // Calculate rotated corners
    p1.x = center.x + cos_a * (-halfWidth) - sin_a * (-halfHeight);
    p1.y = center.y + sin_a * (-halfWidth) + cos_a * (-halfHeight);
    
    p2.x = center.x + cos_a * (halfWidth) - sin_a * (-halfHeight);
    p2.y = center.y + sin_a * (halfWidth) + cos_a * (-halfHeight);
    
    p3.x = center.x + cos_a * (halfWidth) - sin_a * (halfHeight);
    p3.y = center.y + sin_a * (halfWidth) + cos_a * (halfHeight);
    
    p4.x = center.x + cos_a * (-halfWidth) - sin_a * (halfHeight);
    p4.y = center.y + sin_a * (-halfWidth) + cos_a * (halfHeight);
    
    // Draw the rotated rectangle
    draw_list->AddQuad(p1, p2, p3, p4, ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.9f, 1.0f)), 2.0f);
    draw_list->AddQuadFilled(p1, p2, p3, p4, ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.9f, 0.2f)));
    
    // Add "TOP" label to indicate orientation
    ImVec2 topCenter((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
    draw_list->AddText(topCenter, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), "TOP");
    
    ImGui::Dummy(previewSize);
}

void VideoEditor::renderCropControls(FFmpegDecoder& decoder) {
    ImGui::Text("Crop Video");
    
    // Toggle for crop mode
    if (ImGui::Checkbox("Enable Crop", &isCropActive)) {
        if (!isCropActive) {
            resetCropState();
        }
    }
    
    if (isCropActive) {
        // Numerical inputs for crop rectangle
        ImGui::Text("Crop Rectangle (normalized coordinates):");
        ImGui::SliderFloat("Left", &cropRect.x, 0.0f, cropRect.x + cropRect.z - 0.1f, "%.2f");
        ImGui::SliderFloat("Top", &cropRect.y, 0.0f, cropRect.y + cropRect.w - 0.1f, "%.2f");
        ImGui::SliderFloat("Width", &cropRect.z, 0.1f, 1.0f - cropRect.x, "%.2f");
        ImGui::SliderFloat("Height", &cropRect.w, 0.1f, 1.0f - cropRect.y, "%.2f");
        
        // Aspect ratio lock option
        static bool lockAspectRatio = true;
        ImGui::Checkbox("Lock Aspect Ratio", &lockAspectRatio);
        
        // Draw crop preview
        ImGui::Text("Preview:");
        
        ImVec2 previewPos = ImGui::GetCursorScreenPos();
        ImVec2 previewSize(300, 200);
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        // Draw video frame background
        draw_list->AddRectFilled(
            previewPos,
            ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y),
            ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f))
        );
        
        // Draw crop rectangle
        ImVec2 cropStart(
            previewPos.x + cropRect.x * previewSize.x,
            previewPos.y + cropRect.y * previewSize.y
        );
        ImVec2 cropEnd(
            previewPos.x + (cropRect.x + cropRect.z) * previewSize.x,
            previewPos.y + (cropRect.y + cropRect.w) * previewSize.y
        );
        
        // Draw the non-cropped area as semi-transparent
        draw_list->AddRectFilled(
            previewPos,
            ImVec2(previewPos.x + previewSize.x, previewPos.y + previewSize.y),
            ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.7f))
        );
        
        // Draw the crop area (clear)
        draw_list->AddRectFilled(
            cropStart,
            cropEnd,
            ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.0f))
        );
        
        // Draw border around crop area
        draw_list->AddRect(
            cropStart,
            cropEnd,
            ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)),
            0.0f,
            15,
            2.0f
        );
        
        // Draw corner handles
        float handleSize = 8.0f;
        ImVec2 corners[4] = {
            cropStart,
            ImVec2(cropEnd.x, cropStart.y),
            cropEnd,
            ImVec2(cropStart.x, cropEnd.y)
        };
        
        for (int i = 0; i < 4; i++) {
            draw_list->AddRectFilled(
                ImVec2(corners[i].x - handleSize/2, corners[i].y - handleSize/2),
                ImVec2(corners[i].x + handleSize/2, corners[i].y + handleSize/2),
                ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
            );
        }
        
        ImGui::Dummy(previewSize);
        
        // Handle mouse interactions with the crop rectangle
        ImVec2 mousePos = ImGui::GetMousePos();
        bool isMouseInPreview = 
            mousePos.x >= previewPos.x && mousePos.x <= previewPos.x + previewSize.x &&
            mousePos.y >= previewPos.y && mousePos.y <= previewPos.y + previewSize.y;
        
        // Detect if mouse is near any corner
        if (isMouseInPreview && !isDraggingCrop) {
            for (int i = 0; i < 4; i++) {
                if (mousePos.x >= corners[i].x - handleSize && mousePos.x <= corners[i].x + handleSize &&
                    mousePos.y >= corners[i].y - handleSize && mousePos.y <= corners[i].y + handleSize) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                    
                    if (ImGui::IsMouseClicked(0)) {
                        isDraggingCrop = true;
                        dragCorner = i + 1;  // 1-4 for corners
                    }
                    
                    break;
                }
            }
        }
        
        // Handle dragging
        if (isDraggingCrop) {
            if (ImGui::IsMouseDown(0)) {
                // Normalize mouse position
                float normX = (mousePos.x - previewPos.x) / previewSize.x;
                float normY = (mousePos.y - previewPos.y) / previewSize.y;
                
                // Clamp to [0,1]
                normX = ImClamp(normX, 0.0f, 1.0f);
                normY = ImClamp(normY, 0.0f, 1.0f);
                
                // Update crop rectangle based on which corner is being dragged
                switch (dragCorner) {
                    case 1: { // Top-left
                        float oldRight = cropRect.x + cropRect.z;
                        float oldBottom = cropRect.y + cropRect.w;
                        
                        cropRect.x = normX;
                        cropRect.y = normY;
                        cropRect.z = oldRight - cropRect.x;
                        cropRect.w = oldBottom - cropRect.y;
                        break;
                    }
                    case 2: { // Top-right
                        float oldBottom = cropRect.y + cropRect.w;
                        
                        cropRect.y = normY;
                        cropRect.z = normX - cropRect.x;
                        cropRect.w = oldBottom - cropRect.y;
                        break;
                    }
                    case 3: { // Bottom-right
                        cropRect.z = normX - cropRect.x;
                        cropRect.w = normY - cropRect.y;
                        break;
                    }
                    case 4: { // Bottom-left
                        float oldRight = cropRect.x + cropRect.z;
                        
                        cropRect.x = normX;
                        cropRect.z = oldRight - cropRect.x;
                        cropRect.w = normY - cropRect.y;
                        break;
                    }
                }
                
                // Ensure minimum size
                if (cropRect.z < 0.1f) cropRect.z = 0.1f;
                if (cropRect.w < 0.1f) cropRect.w = 0.1f;
                
                // Ensure within bounds
                if (cropRect.x + cropRect.z > 1.0f) cropRect.x = 1.0f - cropRect.z;
                if (cropRect.y + cropRect.w > 1.0f) cropRect.y = 1.0f - cropRect.w;
                
                // Lock aspect ratio if enabled
                if (lockAspectRatio) {
                    // Determine which dimension to prioritize based on drag direction
                    bool prioritizeWidth = (dragCorner == 2 || dragCorner == 3);
                    
                    float aspectRatio = decoder.getWidth() / static_cast<float>(decoder.getHeight());
                    
                    if (prioritizeWidth) {
                        cropRect.w = cropRect.z / aspectRatio;
                    } else {
                        cropRect.z = cropRect.w * aspectRatio;
                    }
                    
                    // Ensure within bounds after adjusting
                    if (cropRect.x + cropRect.z > 1.0f) {
                        cropRect.z = 1.0f - cropRect.x;
                        cropRect.w = cropRect.z / aspectRatio;
                    }
                    if (cropRect.y + cropRect.w > 1.0f) {
                        cropRect.w = 1.0f - cropRect.y;
                        cropRect.z = cropRect.w * aspectRatio;
                    }
                }
            } else {
                // Mouse released
                isDraggingCrop = false;
                dragCorner = 0;
            }
        }
    }
}

void VideoEditor::renderStitchControls() {
    ImGui::Text("Stitch Videos Together");
    
    // Add a new clip button
    if (ImGui::Button("Add Clip")) {
        // In a real implementation, this would open a file dialog
        std::string filePath = "sample_clip.mp4";
        
        VideoClip clip;
        clip.filePath = filePath;
        clip.startTime = 0.0f;
        clip.endTime = 60.0f;  // Assume 60 seconds as default
        clip.rotationAngle = 0;
        clip.cropRect = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);  // No crop
        
        stitchClips.push_back(clip);
        selectedClipIndex = stitchClips.size() - 1;
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clear All")) {
        stitchClips.clear();
        selectedClipIndex = -1;
    }
    
    ImGui::Separator();
    
    // List of clips
    ImGui::Text("Clips (%zu):", stitchClips.size());
    ImGui::BeginChild("StitchClips", ImVec2(0, 200), true);
    
    for (int i = 0; i < stitchClips.size(); i++) {
        auto& clip = stitchClips[i];
        
        // Generate display name from path
        std::filesystem::path path(clip.filePath);
        std::string clipName = path.filename().string();
        
        // Create a unique ID for each item
        ImGui::PushID(i);
        
        bool isSelected = (i == selectedClipIndex);
        if (ImGui::Selectable(clipName.c_str(), isSelected)) {
            selectedClipIndex = i;
        }
        
        // Drag and drop for reordering
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("STITCH_CLIP", &i, sizeof(int));
            ImGui::Text("Move %s", clipName.c_str());
            ImGui::EndDragDropSource();
        }
        
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STITCH_CLIP")) {
                int sourceIndex = *(const int*)payload->Data;
                
                // Swap clips
                if (sourceIndex != i) {
                    VideoClip temp = stitchClips[sourceIndex];
                    stitchClips.erase(stitchClips.begin() + sourceIndex);
                    
                    // Adjust target index if source was before it
                    int targetIndex = (sourceIndex < i) ? i - 1 : i;
                    stitchClips.insert(stitchClips.begin() + targetIndex, temp);
                    
                    // Update selected index
                    if (selectedClipIndex == sourceIndex) {
                        selectedClipIndex = targetIndex;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
        
        ImGui::PopID();
    }
    
    ImGui::EndChild();
    
    ImGui::Separator();
    
    // Selected clip details
    if (selectedClipIndex >= 0 && selectedClipIndex < stitchClips.size()) {
        auto& clip = stitchClips[selectedClipIndex];
        
        ImGui::Text("Selected Clip: %s", std::filesystem::path(clip.filePath).filename().string().c_str());
        
        // Trim range for the clip
        ImGui::Text("Trim Range:");
        ImGui::SliderFloat("Start Time (s)", &clip.startTime, 0.0f, clip.endTime - 0.1f);
        ImGui::SliderFloat("End Time (s)", &clip.endTime, clip.startTime + 0.1f, 300.0f);
        
        // Move up/down buttons
        if (ImGui::Button("Move Up") && selectedClipIndex > 0) {
            std::swap(stitchClips[selectedClipIndex], stitchClips[selectedClipIndex - 1]);
            selectedClipIndex--;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Move Down") && selectedClipIndex < stitchClips.size() - 1) {
            std::swap(stitchClips[selectedClipIndex], stitchClips[selectedClipIndex + 1]);
            selectedClipIndex++;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Remove")) {
            stitchClips.erase(stitchClips.begin() + selectedClipIndex);
            if (selectedClipIndex >= stitchClips.size()) {
                selectedClipIndex = stitchClips.empty() ? -1 : stitchClips.size() - 1;
            }
        }
    } else if (!stitchClips.empty()) {
        ImGui::Text("Select a clip to edit its properties.");
    } else {
        ImGui::Text("Add clips to begin stitching.");
    }
}

void VideoEditor::applyEdits(FFmpegDecoder& decoder) {
    const std::string& currentFile = decoder.getFilename();
    
    switch (currentOperation) {
        case EditOperation::Trim:
            if (isTrimActive && hasSelectedStartTime && hasSelectedEndTime) {
                // Check if trim times are valid
                if (trimStartTime < trimEndTime) {
                    std::string outputFile = generateOutputFilename(currentFile, "trimmed");
                    
                    // Validate that we have actual file paths
                    if (!currentFile.empty() && !outputFile.empty()) {
                        std::cout << "Trimming video from " << trimStartTime << "s to " << trimEndTime << "s" << std::endl;
                        std::cout << "Output file: " << outputFile << std::endl;
                        
                        // Call FFmpeg to trim the video
                        trimVideo(currentFile, outputFile, trimStartTime, trimEndTime);
                    }
                } else {
                    // Error - end time is before start time
                    std::cerr << "Error: End time must be after start time" << std::endl;
                }
            }
            break;
            
        case EditOperation::Rotate:
            if (rotationAngle != 0) {
                std::string outputFile = generateOutputFilename(currentFile, "rotated_" + std::to_string(rotationAngle));
                
                if (!currentFile.empty() && !outputFile.empty()) {
                    std::cout << "Rotating video by " << rotationAngle << " degrees" << std::endl;
                    std::cout << "Output file: " << outputFile << std::endl;
                    
                    rotateVideo(currentFile, outputFile, rotationAngle);
                }
            }
            break;
            
        case EditOperation::Crop:
            if (isCropActive) {
                std::string outputFile = generateOutputFilename(currentFile, "cropped");
                
                if (!currentFile.empty() && !outputFile.empty()) {
                    std::cout << "Cropping video to: " 
                        << cropRect.x << ", " << cropRect.y << ", " 
                        << cropRect.z << ", " << cropRect.w << std::endl;
                    std::cout << "Output file: " << outputFile << std::endl;
                    
                    cropVideo(currentFile, outputFile, cropRect.x, cropRect.y, cropRect.z, cropRect.w);
                }
            }
            break;
            
        case EditOperation::Stitch:
            if (!stitchClips.empty()) {
                std::string outputFile = generateOutputFilename(currentFile, "stitched");
                
                std::vector<std::string> inputFiles;
                for (const auto& clip : stitchClips) {
                    inputFiles.push_back(clip.filePath);
                }
                
                if (!outputFile.empty() && !inputFiles.empty()) {
                    std::cout << "Stitching " << inputFiles.size() << " clips together" << std::endl;
                    std::cout << "Output file: " << outputFile << std::endl;
                    
                    stitchVideos(inputFiles, outputFile);
                }
            }
            break;
            
        default:
            break;
    }
    
    // Reset state after successful edit
    resetEditorState();
}

void VideoEditor::trimVideo(const std::string& inputFile, const std::string& outputFile, 
                          float startTime, float endTime) {
    // This would typically use FFmpeg's C API or execute an FFmpeg command
    // For now, we're just printing what would happen
    std::cout << "Executing FFmpeg trim command:" << std::endl;
    std::cout << "ffmpeg -i \"" << inputFile << "\" -ss " << startTime 
              << " -to " << endTime << " -c copy \"" << outputFile << "