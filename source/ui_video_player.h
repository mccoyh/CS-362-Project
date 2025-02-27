#pragma once

#include <string>
#include <memory>
#include <vector>
#include <imgui.h>
// #include <SDL3/SDL.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class UIVideoPlayer {
public:
    // Supported playback speeds
    static const std::vector<float> PLAYBACK_SPEEDS;

private:
    // Video playback state
    struct VideoState {
        // File and playback details
        std::string filepath;
        std::string filename;

        // Playback controls
        bool isPlaying;
        bool isPaused;
        float playbackSpeed;
        float volume;

        // Video metadata
        int width;
        int height;
        double duration;
        double currentPosition;

        // FFmpeg components
        AVFormatContext* formatContext;
        AVCodecContext* codecContext;
        AVStream* videoStream;
        SwsContext* swsContext;

        // SDL rendering
        // SDL_Texture* texture;
        // SDL_Renderer* renderer;
    };

    // Current video state
    std::unique_ptr<VideoState> currentVideo;

    // Subtitle management
    class SubtitleManager {
    public:
        void loadSubtitles(const std::string& subtitlePath);
        void renderSubtitles(double currentTime);
        void setLanguage(const std::string& language);
    };
    SubtitleManager subtitleManager;

    // Internal methods
    void initializePlayback(const std::string& filepath);
    void cleanupPlayback();
    void decodeVideoFrame();
    void updateVideoFrame();

    // UI Rendering methods
    void renderPlaybackControls();
    void renderVideoDisplay();
    void renderPlaybackTimeline();
    void renderVolumeControl();

public:
    UIVideoPlayer();
    ~UIVideoPlayer();

    // Main rendering method
    void render();

    // Video loading and control methods
    bool loadVideo(const std::string& filepath);
    void play();
    void pause();
    void stop();
    void seek(double position);

    // Playback configuration
    void setPlaybackSpeed(float speed);
    void adjustVolume(float volume);

    // Subtitle methods
    void toggleSubtitles();
    void setSubtitleLanguage(const std::string& language);

    // Query methods
    bool isVideoLoaded() const;
    std::string getCurrentVideoPath() const;
};