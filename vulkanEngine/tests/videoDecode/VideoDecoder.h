#ifndef VIDEODECODER_H
#define VIDEODECODER_H

extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libswscale/swscale.h>
}
#include <vector>

class VideoDecoder {
public:
  explicit VideoDecoder(const char* filename);

  ~VideoDecoder();

  bool getNextFrame(std::vector<uint8_t>& outData, int& outWidth, int& outHeight) const;

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
