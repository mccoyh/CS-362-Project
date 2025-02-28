#ifndef AVPARSER_H
#define AVPARSER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include <vector>
#include <string>

namespace AVParser {

struct AVFrameData {
  std::vector<uint8_t> videoData;
  std::vector<uint8_t> audioData;
  int frameWidth;
  int frameHeight;
};

class AVParser {
public:
  explicit AVParser(const std::string& mediaFile);

  ~AVParser();

  [[nodiscard]] AVFrameData getCurrentFrame() const;

  [[nodiscard]] double getFrameRate() const;

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

  [[nodiscard]] int getFrameWidth() const;

  [[nodiscard]] int getFrameHeight() const;

  void findStreamIndices();

  void setupVideo();

  void setupAudio();

  void validateVideoContext() const;
};

} // AVParser

#endif //AVPARSER_H
