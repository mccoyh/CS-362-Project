#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <imgui.h>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}
class UIVideoEditor {
public:
    // Editing modes
    enum class EditMode {
        Trim,
        Crop,
        Rotate,
        AdjustColor
    };
    // Video editing item structure
    struct EditableVideo {
        std::string filepath;
        std::string filename;
        
        // Video metadata
        int width;
        int height;
        double duration;
        
        // Editing parameters
        double trimStart;
        double trimEnd;
        int rotationAngle;
        
        // Crop parameters
        int cropX;
        int cropY;
        int cropWidth;
        int cropHeight;
        
        // Color adjustments
        float brightness;
        float contrast;
        float saturation;
    };
private:
    // List of videos in editing queue
    std::vector<EditableVideo> editQueue;
    // Current selected video for editing
    int currentVideoIndex;
    // Current editing mode
    EditMode currentEditMode;
    // FFmpeg video processing context
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    SwsContext* swsContext;
    // Helper methods for video processing
    bool loadVideoMetadata(EditableVideo& video);
    bool processVideoEdit(const EditableVideo& video, const std::string& outputPath);
    void resetVideoParameters(EditableVideo& video);
    // Rendering helper methods
    void renderEditModeSelector();
    void renderTrimControls();
    void renderCropControls();
    void renderRotationControls();
    void renderColorAdjustmentControls();
public:
    UIVideoEditor();
    ~UIVideoEditor();
    // Main rendering method
    void render();
    // Video editing methods
    void addVideoToEditQueue(const std::string& filepath);
    void removeVideoFromEditQueue(int index);
    void clearEditQueue();
    // Specific editing operations
    void trimVideo(double start, double end);
    void rotateVideo(int angle);
    void cropVideo(int x, int y, int width, int height);
    void adjustColor(float brightness, float contrast, float saturation);
    // Export edited video
    bool exportEditedVideo(const std::string& outputPath);
    // Utility methods
    bool isVideoSelected() const;
    EditableVideo& getCurrentVideo();
};
// Utility class for video file operations
class VideoFileUtils {
public:
    // Check if file is a valid video
    static bool isValidVideoFile(const std::string& filepath);
    // Get video file metadata
    static bool getVideoMetadata(const std::string& filepath, 
                                 int& width, 
                                 int& height, 
                                 double& duration);
    // Generate unique filename for edited video
    static std::string generateUniqueFilename(const std::string& originalPath);
};
