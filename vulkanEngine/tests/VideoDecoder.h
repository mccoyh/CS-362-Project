#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <vector>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;
struct AVCodec;

class VideoDecoder {
public:
  explicit VideoDecoder(const char* filename);

  ~VideoDecoder();

  bool getNextFrame(std::vector<uint8_t>& outData, int& outWidth, int& outHeight) const;

  [[nodiscard]] double getFrameRate() const;

  [[nodiscard]] int getWidth() const;

  [[nodiscard]] int getHeight() const;

private:
  AVFormatContext* fmtCtx = nullptr;
  AVCodecContext* codecCtx = nullptr;
  AVFrame* frame = nullptr;
  AVPacket* packet = nullptr;
  SwsContext* swsCtx = nullptr;
  const AVCodec* codec = nullptr;
  int videoStream = -1;
};



#endif //VIDEODECODER_H
