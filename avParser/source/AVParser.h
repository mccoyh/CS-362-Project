#ifndef AVPARSER_H
#define AVPARSER_H

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <cstdint>

struct AVFormatContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;
struct AVCodec;
struct AVCodecContext;

namespace AVParser {

struct AVFrameData {
  std::shared_ptr<std::vector<uint8_t>> videoData;
  std::shared_ptr<std::vector<uint8_t>> audioData;
  int frameWidth;
  int frameHeight;
};

enum class MediaState {
  AUTO_PLAYING,
  PAUSED,
  MANUAL
};

class MediaParser {
public:
  explicit MediaParser(const std::string& mediaFile);

  ~MediaParser();

  [[nodiscard]] AVFrameData getCurrentFrame() const;

  [[nodiscard]] double getFrameRate() const;

  [[nodiscard]] uint32_t getTotalFrames() const;

  [[nodiscard]] uint32_t getCurrentFrameIndex() const;

  void loadNextFrame();

  void loadPreviousFrame();

  void loadFrameAt(uint32_t targetFrame);

  void update();

  void play();

  void pause();

  void setManual(bool manual);

  [[nodiscard]] MediaState getState() const;

private:
  AVFormatContext* formatContext = nullptr;
  AVFrame* frame = nullptr;
  AVPacket* packet = nullptr;
  SwsContext* swsContext = nullptr;

  const AVCodec* videoCodec = nullptr;
  AVCodecContext* videoCodecContext = nullptr;

  const AVCodec* audioCodec = nullptr;
  AVCodecContext* audioCodecContext = nullptr;

  int videoStreamIndex = -1;
  int audioStreamIndex = -1;

  uint32_t currentFrame;

  std::shared_ptr<std::vector<uint8_t>> currentVideoData;
  std::shared_ptr<std::vector<uint8_t>> currentAudioData;

  float timeAccumulator = 0;
  std::chrono::time_point<std::chrono::steady_clock> previousTime;

  MediaState state = MediaState::AUTO_PLAYING;

  std::map<int, bool> keyFrameMap;
  struct FrameCache {
    std::unordered_map<uint32_t, std::vector<uint8_t>> frames;
  };
  std::unordered_map<uint32_t, FrameCache> cache;

  uint32_t totalFrames = 0;

  [[nodiscard]] int getFrameWidth() const;

  [[nodiscard]] int getFrameHeight() const;

  void findStreamIndices();

  void setupVideo();

  void loadVideoKeyframes();

  void calculateTotalFrames();

  void setupAudio();

  void validateVideoContext() const;

  void validateVideoStream() const;

  void validateAudioStream() const;

  void seekToFrame(int64_t targetFrame) const;

  void loadFrame() const;

  void convertVideoFrame() const;

  void loadFrameFromCache(uint32_t targetFrame);

  void loadFrames(uint32_t targetFrame);
};
} // AVParser

#endif //AVPARSER_H
