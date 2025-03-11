#ifndef AVPARSER_H
#define AVPARSER_H
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>

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

struct AudioParams {
  int sampleRate = 44100;
  int channels = 2;
  int bitsPerSample = 16;
  double frequency = 420.0; // Frequency in Hz
};

class MediaParser {
public:
  MediaParser(const std::string& mediaFile, const AudioParams& params);

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

  bool getNextAudioChunk(uint8_t*& outBuffer, int& outBufferSize);

  void setFilepath(const std::string& mediaFile);

private:
  AVFormatContext* formatContext = nullptr;
  AVFrame* frame = nullptr;
  AVPacket* packet = nullptr;

  const AVCodec* videoCodec = nullptr;
  AVCodecContext* videoCodecContext = nullptr;
  SwsContext* swsContext = nullptr;

  const AVCodec* audioCodec = nullptr;
  AVCodecContext* audioCodecContext = nullptr;
  SwrContext* swrContext = nullptr;

  int videoStreamIndex = -1;
  int audioStreamIndex = -1;

  uint32_t currentFrame;

  std::shared_ptr<std::vector<uint8_t>> currentVideoData;
  std::shared_ptr<std::vector<uint8_t>> currentAudioData;

  std::shared_ptr<std::vector<uint8_t>> backgroundVideoData;

  float timeAccumulator = 0;
  std::chrono::time_point<std::chrono::steady_clock> previousTime;

  MediaState state = MediaState::AUTO_PLAYING;

  std::map<int, bool> keyFrameMap;

  using FrameCache = std::vector<std::vector<uint8_t>>;
  std::unordered_map<uint32_t, FrameCache> videoCache;

  uint32_t totalFrames = 0;

  AudioParams params;

  std::map<uint32_t, std::vector<uint8_t>> audioCache;
  std::map<uint32_t, uint32_t> audioCacheSizes;
  uint32_t currentAudioChunk = 0;

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

  void loadFrame();

  void convertVideoFrame() const;

  void loadFrameFromCache(uint32_t targetFrame);

  void loadFrames(uint32_t targetFrame);

  bool decodeAudioChunk(uint8_t*& outBuffer, int& outBufferSize);

  std::atomic<bool> keepLoadingInBackground = true;
  std::thread backgroundThread;

  void backgroundFrameLoader();
};
} // AVParser

#endif //AVPARSER_H
