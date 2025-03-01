#include "AVParser.h"
#include <stdexcept>

namespace AVParser {
  MediaParser::MediaParser(const std::string& mediaFile)
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

  MediaParser::~MediaParser()
  {
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&audioCodecContext);
    avcodec_free_context(&videoCodecContext);
    avformat_close_input(&formatContext);
    sws_freeContext(swsContext);
  }

  AVFrameData MediaParser::getCurrentFrame() const
  {
    return {
      .videoData = currentVideoData,
      .audioData = currentAudioData,
      .frameWidth = getFrameWidth(),
      .frameHeight = getFrameHeight()
    };
  }

  double MediaParser::getFrameRate() const
  {
    if (videoStreamIndex == -1)
    {
      throw std::runtime_error("No video stream available!");
    }

    const AVRational fps = formatContext->streams[videoStreamIndex]->r_frame_rate;

    return av_q2d(fps);
  }

  int MediaParser::getFrameWidth() const
  {
    validateVideoContext();

    return videoCodecContext->width;
  }

  int MediaParser::getFrameHeight() const
  {
    validateVideoContext();

    return videoCodecContext->height;
  }

  void MediaParser::findStreamIndices()
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

  void MediaParser::setupVideo()
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
      throw std::runtime_error("Failed to open video codec!");
    }

    swsContext = sws_getContext(videoCodecContext->width, videoCodecContext->height,
                                videoCodecContext->pix_fmt, videoCodecContext->width,
                                videoCodecContext->height, AV_PIX_FMT_RGBA, SWS_BILINEAR,
                                nullptr, nullptr, nullptr);
  }

  void MediaParser::setupAudio()
  {
    audioCodec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
    if (!audioCodec)
    {
      throw std::runtime_error("Failed to find audio decoder!");
    }

    audioCodecContext = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_to_context(audioCodecContext, formatContext->streams[audioStreamIndex]->codecpar);

    if (avcodec_open2(audioCodecContext, audioCodec, nullptr) < 0)
    {
      throw std::runtime_error("Failed to open audio codec!");
    }
  }

  void MediaParser::validateVideoContext() const
  {
    if (!videoCodecContext)
    {
      throw std::runtime_error("Codec context not initialized!");
    }
  }
} // AVParser