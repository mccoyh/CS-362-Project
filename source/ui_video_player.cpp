#include "ui_video_player.h"
// #include "../include/config.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

// Supported playback speeds
const std::vector<float> UIVideoPlayer::PLAYBACK_SPEEDS = {
    0.25f, 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 2.0f
};

UIVideoPlayer::UIVideoPlayer() : 
    currentVideo(std::make_unique<VideoState>()) {
    // Initialize default video state
    currentVideo->isPlaying = false;
    currentVideo->isPaused = false;
    currentVideo->playbackSpeed = 1.0f;
    currentVideo->volume = 0.5f;
    
    // Initialize FFmpeg and SDL components to nullptr
    currentVideo->formatContext = nullptr;
    currentVideo->codecContext = nullptr;
    currentVideo->videoStream = nullptr;
    currentVideo->swsContext = nullptr;
    // currentVideo->texture = nullptr;
    // currentVideo->renderer = nullptr;
}

UIVideoPlayer::~UIVideoPlayer() {
    cleanupPlayback();
}

void UIVideoPlayer::render() {
    ImGui::Begin("Video Player");

    // Render video display
    renderVideoDisplay();

    // Render playback controls
    renderPlaybackControls();

    // Render playback timeline
    renderPlaybackTimeline();

    // Render volume control
    renderVolumeControl();

    ImGui::End();
}

bool UIVideoPlayer::loadVideo(const std::string& filepath) {
    // Validate file format
    // if (!MediaPlayerConfig::isSupportedVideoFormat(filepath)) {
        // std::cerr << "Unsupported video format: " << filepath << std::endl;
        // return false;
    // }

    // Cleanup any existing playback
    cleanupPlayback();

    try {
        // Initialize FFmpeg components
        initializePlayback(filepath);
        
        // Update current video state
        currentVideo->filepath = filepath;
        // currentVideo->filename = std::filesystem::path(filepath).filename().string();
        currentVideo->isPlaying = true;
        currentVideo->isPaused = false;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load video: " << e.what() << std::endl;
        return false;
    }
}

void UIVideoPlayer::initializePlayback(const std::string& filepath) {
    // Open input file
    if (avformat_open_input(&currentVideo->formatContext, filepath.c_str(), nullptr, nullptr) != 0) {
        throw std::runtime_error("Could not open input file");
    }

    // Retrieve stream information
    if (avformat_find_stream_info(currentVideo->formatContext, nullptr) < 0) {
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Could not find stream information");
    }

    // Find the first video stream
    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < currentVideo->formatContext->nb_streams; i++) {
        if (currentVideo->formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Could not find video stream");
    }

    // Get video stream
    currentVideo->videoStream = currentVideo->formatContext->streams[videoStreamIndex];

    // Find decoder
    const AVCodec* codec = avcodec_find_decoder(currentVideo->videoStream->codecpar->codec_id);
    if (!codec) {
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Unsupported codec");
    }

    // Allocate codec context
    currentVideo->codecContext = avcodec_alloc_context3(codec);
    if (!currentVideo->codecContext) {
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Failed to allocate codec context");
    }

    // Fill codec context
    if (avcodec_parameters_to_context(currentVideo->codecContext, 
                                      currentVideo->videoStream->codecpar) < 0) {
        avcodec_free_context(&currentVideo->codecContext);
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Failed to copy codec parameters");
    }

    // Open codec
    if (avcodec_open2(currentVideo->codecContext, codec, nullptr) < 0) {
        avcodec_free_context(&currentVideo->codecContext);
        avformat_close_input(&currentVideo->formatContext);
        throw std::runtime_error("Failed to open codec");
    }

    // Set video metadata
    currentVideo->width = currentVideo->codecContext->width;
    currentVideo->height = currentVideo->codecContext->height;
    currentVideo->duration = currentVideo->formatContext->duration / 
                             static_cast<double>(AV_TIME_BASE);
}

void UIVideoPlayer::cleanupPlayback() {
    // Free FFmpeg resources
    if (currentVideo->formatContext) {
        avformat_close_input(&currentVideo->formatContext);
        currentVideo->formatContext = nullptr;
    }

    if (currentVideo->codecContext) {
        avcodec_free_context(&currentVideo->codecContext);
        currentVideo->codecContext = nullptr;
    }

    if (currentVideo->swsContext) {
        sws_freeContext(currentVideo->swsContext);
        currentVideo->swsContext = nullptr;
    }

    // Free SDL resources
    // if (currentVideo->texture) {
    //     SDL_DestroyTexture(currentVideo->texture);
    //     currentVideo->texture = nullptr;
    // }
    //
    // if (currentVideo->renderer) {
    //     SDL_DestroyRenderer(currentVideo->renderer);
    //     currentVideo->renderer = nullptr;
    // }

    // Reset video state
    currentVideo->isPlaying = false;
    currentVideo->isPaused = false;
    currentVideo->filepath.clear();
    currentVideo->filename.clear();
}

void UIVideoPlayer::renderPlaybackControls() {
    ImGui::BeginGroup();

    // Play/Pause button
    if (ImGui::Button(currentVideo->isPlaying && !currentVideo->isPaused ? "Pause" : "Play")) {
        if (!currentVideo->isPlaying) {
            play();
        } else {
            pause();
        }
    }
    ImGui::SameLine();

    // Stop button
    if (ImGui::Button("Stop")) {
        stop();
    }
    ImGui::SameLine();

    // Playback speed dropdown
    if (ImGui::BeginCombo("Speed", std::to_string(currentVideo->playbackSpeed).c_str())) {
        for (float speed : PLAYBACK_SPEEDS) {
            bool isSelected = currentVideo->playbackSpeed == speed;
            if (ImGui::Selectable(std::to_string(speed).c_str(), isSelected)) {
                setPlaybackSpeed(speed);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::EndGroup();
}

void UIVideoPlayer::renderVideoDisplay() {
    // Placeholder for video rendering
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImGui::Text("Video Display Placeholder");
    ImGui::Dummy(windowSize);
}

void UIVideoPlayer::renderPlaybackTimeline() {
    if (!isVideoLoaded()) return;

    // Playback position slider
    float currentPos = currentVideo->currentPosition;
    float duration = currentVideo->duration;

    ImGui::SliderFloat("Position", &currentPos, 0.0f, duration, "%.2f sec");
    
    // Update current position if slider changed
    if (ImGui::IsItemActive()) {
        seek(currentPos);
    }
}

void UIVideoPlayer::renderVolumeControl() {
    float volume = currentVideo->volume;
    ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
    
    if (volume != currentVideo->volume) {
        adjustVolume(volume);
    }
}

void UIVideoPlayer::play() {
    if (!isVideoLoaded()) return;
    
    currentVideo->isPlaying = true;
    currentVideo->isPaused = false;
}

void UIVideoPlayer::pause() {
    if (!isVideoLoaded()) return;
    
    currentVideo->isPaused = true;
}

void UIVideoPlayer::stop() {
    if (!isVideoLoaded()) return;
    
    currentVideo->isPlaying = false;
    currentVideo->isPaused = false;
    currentVideo->currentPosition = 0.0;
}

void UIVideoPlayer::seek(double position) {
    if (!isVideoLoaded()) return;
    
    // Clamp position to video duration
    position = std::max(0.0, std::min(position, currentVideo->duration));
    
    currentVideo->currentPosition = position;
    // TODO: Implement actual seeking in FFmpeg
}

void UIVideoPlayer::setPlaybackSpeed(float speed) {
    if (!isVideoLoaded()) return;
    
    currentVideo->playbackSpeed = speed;
    // TODO: Implement actual playback speed change in FFmpeg
}

void UIVideoPlayer::adjustVolume(float volume) {
    if (!isVideoLoaded()) return;
    
    currentVideo->volume = std::clamp(volume, 0.0f, 1.0f);
    // TODO: Implement actual volume adjustment
}

bool UIVideoPlayer::isVideoLoaded() const {
    return !currentVideo->filepath.empty() && 
           currentVideo->formatContext != nullptr && 
           currentVideo->codecContext != nullptr;
}

std::string UIVideoPlayer::getCurrentVideoPath() const {
    return currentVideo->filepath;
}

// Subtitle Manager Implementation
void UIVideoPlayer::SubtitleManager::loadSubtitles(const std::string& subtitlePath) {
    // TODO: Implement subtitle loading logic
    std::cout << "Loading subtitles from: " << subtitlePath << std::endl;
}

void UIVideoPlayer::SubtitleManager::renderSubtitles(double currentTime) {
    // TODO: Implement subtitle rendering
    // This would involve finding the correct subtitle for the current video time
    // and rendering it on screen
}

void UIVideoPlayer::SubtitleManager::setLanguage(const std::string& language) {
    // TODO: Implement subtitle language selection
    std::cout << "Setting subtitle language to: " << language << std::endl;
}

void UIVideoPlayer::toggleSubtitles() {
    // TODO: Implement subtitle toggling
    std::cout << "Toggling subtitles" << std::endl;
}

void UIVideoPlayer::setSubtitleLanguage(const std::string& language) {
    subtitleManager.setLanguage(language);
}