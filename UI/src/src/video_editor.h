#ifndef VIDEO_EDITOR_H
#define VIDEO_EDITOR_H

#include <string>
#include <vector>
#include "imgui.h"
#include "ffmpeg_decoder.h"

enum class EditOperation {
    None,
    Trim,
    Rotate,
    Crop,
    Stitch
};

struct VideoClip {
    std::string filePath;
    float startTime;
    float endTime;
    int rotationAngle;  // 0, 90, 180, 270 degrees
    ImVec4 cropRect;    // x,y = top-left, z,w = width,height (normalized 0-1)
};

class VideoEditor {
public:
    VideoEditor();
    ~VideoEditor();
    
    // Main function to render the editor UI
    void renderEditor(FFmpegDecoder& decoder);
    
private:
    // UI rendering functions
    void renderTrimControls(FFmpegDecoder& decoder);
    void renderRotateControls();
    void renderCropControls(FFmpegDecoder& decoder);
    void renderStitchControls();
    
    // Edit operation functions
    void applyEdits(FFmpegDecoder& decoder);
    void trimVideo(const std::string& inputFile, const std::string& outputFile, 
                  float startTime, float endTime);
    void rotateVideo(const std::string& inputFile, const std::string& outputFile, 
                    int angle);
    void cropVideo(const std::string& inputFile, const std::string& outputFile, 
                  float x, float y, float width, float height);
    void stitchVideos(const std::vector<std::string>& inputFiles, const std::string& outputFile);
    
    // Utility functions
    std::string generateOutputFilename(const std::string& inputFile, const std::string& operation);
    void resetTrimState();
    void resetRotateState();
    void resetCropState();
    void resetStitchState();
    void resetEditorState();
    
    // Member variables for editing state
    EditOperation currentOperation;
    
    // Trim feature variables
    bool isTrimActive;
    float trimStartTime;
    float trimEndTime;
    bool hasSelectedStartTime;
    bool hasSelectedEndTime;
    
    // Rotate feature variables
    int rotationAngle;
    
    // Crop feature variables
    bool isCropActive;
    ImVec4 cropRect;  // x,y = top-left, z,w = width,height (normalized 0-1)
    bool isDraggingCrop;
    int dragCorner;  // 0=none, 1=top-left, 2=top-right, 3=bottom-right, 4=bottom-left
    
    // Stitch feature variables
    std::vector<VideoClip> stitchClips;
    int selectedClipIndex;
};

#endif // VIDEO_EDITOR_H