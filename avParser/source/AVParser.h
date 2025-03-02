#ifndef AVPARSER_H
#define AVPARSER_H
#include <deque>
#include <map>
#include <unordered_map>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include <vector>
#include <string>
#include <memory>
#include <chrono>

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

  void loadNextFrame();

  void loadPreviousFrame();

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

  std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::vector<uint8_t>>> cache;

  [[nodiscard]] int getFrameWidth() const;

  [[nodiscard]] int getFrameHeight() const;

  void findStreamIndices();

  void setupVideo();

  void setupAudio();

  void validateVideoContext() const;

  void seekToFrame(int64_t targetFrame) const;

  void loadFrame(uint32_t targetFrame) const;

  void convertVideoFrame() const;

  void useCachedFrame(uint32_t targetFrame);

  void loadFrames(uint32_t targetFrame);
};
} // AVParser

#endif //AVPARSER_H
