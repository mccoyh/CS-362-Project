#include "VideoDecoder.h"
#include <stdexcept>

VideoDecoder::VideoDecoder(const char *filename)
{
  if (avformat_open_input(&fmtCtx, filename, nullptr, nullptr) < 0)
  {
    throw std::runtime_error("Failed to open video file!");
  }

  if (avformat_find_stream_info(fmtCtx, nullptr) < 0)
  {
    throw std::runtime_error("Failed to retrieve stream info!");
  }

  for (unsigned int i = 0; i < fmtCtx->nb_streams; i++)
  {
    if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      videoStream = static_cast<int>(i);
      break;
    }
  }

  if (videoStream == -1)
  {
    throw std::runtime_error("No video stream found!");
  }

  const AVCodec* codec = avcodec_find_decoder(fmtCtx->streams[videoStream]->codecpar->codec_id);
  if (!codec)
  {
    throw std::runtime_error("Failed to find video decoder!");
  }

  codecCtx = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(codecCtx, fmtCtx->streams[videoStream]->codecpar);

  if (avcodec_open2(codecCtx, codec, nullptr) < 0)
  {
    throw std::runtime_error("Failed to open codec!");
  }

  frame = av_frame_alloc();
  packet = av_packet_alloc();
  swsCtx = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
                          codecCtx->width, codecCtx->height, AV_PIX_FMT_RGBA,
                          SWS_BILINEAR, nullptr, nullptr, nullptr);
}

VideoDecoder::~VideoDecoder()
{
  av_packet_free(&packet);
  av_frame_free(&frame);
  avcodec_free_context(&codecCtx);
  avformat_close_input(&fmtCtx);
  sws_freeContext(swsCtx);
}

bool VideoDecoder::getNextFrame(std::vector<uint8_t> &outData, int &outWidth, int &outHeight)
{
  if (paused) {
    // When paused, we keep displaying the last frame
    return false;
  }

  while (av_read_frame(fmtCtx, packet) >= 0)
  {
    if (packet->stream_index == videoStream)
    {
      if (avcodec_send_packet(codecCtx, packet) == 0)
      {
        if (avcodec_receive_frame(codecCtx, frame) == 0)
        {
          outWidth = frame->width;
          outHeight = frame->height;
          outData.resize(outWidth * outHeight * 4);

          uint8_t* dst[1] = { outData.data() };
          const int dstStride[1] = { outWidth * 4 };
          sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, dst, dstStride);

          av_packet_unref(packet);
          return true;
        }
      }
    }

    av_packet_unref(packet);
  }

  return false;
}

void VideoDecoder::pause()
{
  paused = true;
}

void VideoDecoder::resume()
{
  paused = false;
}

bool VideoDecoder::isPaused() const
{
  return paused;
}

bool VideoDecoder::seekToTimestamp(int64_t timestamp)
{
  if (timestamp < 0) {
    timestamp = 0;
  }
  
  int flags = AVSEEK_FLAG_FRAME;
  int64_t target_ts = av_rescale_q(timestamp, 
                                  AV_TIME_BASE_Q, 
                                  fmtCtx->streams[videoStream]->time_base);
                                  
  if (av_seek_frame(fmtCtx, videoStream, target_ts, flags) < 0) {
    return false;
  }
  
  avcodec_flush_buffers(codecCtx);
  return true;
}

void VideoDecoder::seekForward(int seconds)
{
  if (!fmtCtx || videoStream < 0) {
    return;
  }
  
  int64_t current_pts = 0;
  if (frame && frame->pts != AV_NOPTS_VALUE) {
    current_pts = frame->pts;
  }
  
  int64_t target_ts = current_pts + seconds * fmtCtx->streams[videoStream]->time_base.den / fmtCtx->streams[videoStream]->time_base.num;
  seekToTimestamp(target_ts);
}

void VideoDecoder::seekBackward(int seconds)
{
  if (!fmtCtx || videoStream < 0) {
    return;
  }
  
  int64_t current_pts = 0;
  if (frame && frame->pts != AV_NOPTS_VALUE) {
    current_pts = frame->pts;
  }
  
  int64_t target_ts = current_pts - seconds * fmtCtx->streams[videoStream]->time_base.den / fmtCtx->streams[videoStream]->time_base.num;
  seekToTimestamp(target_ts);
}

void VideoDecoder::restart()
{
  seekToTimestamp(0);
  paused = false;
}

double VideoDecoder::getFrameRate() const
{
  if (videoStream == -1)
  {
    throw std::runtime_error("No video stream available!");
  }

  const AVRational fps = fmtCtx->streams[videoStream]->r_frame_rate;

  return av_q2d(fps);
}

int VideoDecoder::getWidth() const
{
  if (!codecCtx)
  {
    throw std::runtime_error("Codec context not initialized!");
  }

  return codecCtx->width;
}

int VideoDecoder::getHeight() const
{
  if (!codecCtx)
  {
    throw std::runtime_error("Codec context not initialized!");
  }

  return codecCtx->height;
}
