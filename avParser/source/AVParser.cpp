#include "AVParser.h"
#include <stdexcept>

namespace AVParser {
  AVParser::AVParser(const std::string& mediaFile)
    : currentFrame(0)
  {
    if (avformat_open_input(&formatContext, mediaFile.c_str(), nullptr, nullptr) < 0)
    {
      throw std::runtime_error("Failed to open video file!");
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0)
    {
      throw std::runtime_error("Failed to retrieve stream info!");
    }

    findStreamIndices();

    setupVideo();

    setupAudio();

    frame = av_frame_alloc();
    packet = av_packet_alloc();
  }

  AVParser::~AVParser()
  {
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&audioCodecContext);
    avcodec_free_context(&videoCodecContext);
    avformat_close_input(&formatContext);
    sws_freeContext(swsContext);
  }

  AVFrameData AVParser::getCurrentFrame() const
  {
    return {
      .frameWidth = getFrameWidth(),
      .frameHeight = getFrameHeight()
    };
  }

  double AVParser::getFrameRate() const
  {
    if (videoStreamIndex == -1)
    {
      throw std::runtime_error("No video stream available!");
    }

    const AVRational fps = formatContext->streams[videoStreamIndex]->r_frame_rate;

    return av_q2d(fps);
  }

  int AVParser::getFrameWidth() const
  {
    validateVideoContext();

    return videoCodecContext->width;
  }

  int AVParser::getFrameHeight() const
  {
    validateVideoContext();

    return videoCodecContext->height;
  }

  void AVParser::findStreamIndices()
  {
    for (size_t i = 0; i < formatContext->nb_streams; i++)
    {
      if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
      {
        videoStreamIndex = static_cast<int>(i);
      }
      else if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
      {
        audioStreamIndex = static_cast<int>(i);
      }

      if (videoStreamIndex != -1 && audioStreamIndex != -1)
      {
        break;
      }
    }

    if (videoStreamIndex == -1)
    {
      throw std::runtime_error("No video stream found!");
    }

    if (audioStreamIndex == -1)
    {
      throw std::runtime_error("No audio stream found!");
    }
  }

  void AVParser::setupVideo()
  {
    videoCodec = avcodec_find_decoder(formatContext->streams[videoStreamIndex]->codecpar->codec_id);
    if (!videoCodec)
    {
      throw std::runtime_error("Failed to find video decoder!");
    }

    videoCodecContext = avcodec_alloc_context3(videoCodec);
    avcodec_parameters_to_context(videoCodecContext, formatContext->streams[videoStreamIndex]->codecpar);

    if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0)
    {
      throw std::runtime_error("Failed to open codec!");
    }

    swsContext = sws_getContext(videoCodecContext->width, videoCodecContext->height,
                                videoCodecContext->pix_fmt, videoCodecContext->width,
                                videoCodecContext->height, AV_PIX_FMT_RGBA, SWS_BILINEAR,
                                nullptr, nullptr, nullptr);
  }

  void AVParser::setupAudio()
  {
  }

  void AVParser::validateVideoContext() const
  {
    if (!videoCodecContext)
    {
      throw std::runtime_error("Codec context not initialized!");
    }
  }
} // AVParser