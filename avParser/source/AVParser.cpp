#include "AVParser.h"
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace AVParser {
  MediaParser::MediaParser(const std::string& mediaFile)
    : currentFrame(0), currentVideoData(std::make_shared<std::vector<uint8_t>>()),
      currentAudioData(std::make_shared<std::vector<uint8_t>>()), previousTime(std::chrono::steady_clock::now())
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

    loadNextFrame();
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

  void MediaParser::loadNextFrame() const
  {
    while (av_read_frame(formatContext, packet) >= 0)
    {
      if (packet->stream_index == videoStreamIndex)
      {
        if (avcodec_send_packet(videoCodecContext, packet) == 0)
        {
          if (avcodec_receive_frame(videoCodecContext, frame) == 0)
          {
            int outWidth = frame->width;
            int outHeight = frame->height;
            currentVideoData->resize(outWidth * outHeight * 4);

            uint8_t* dst[1] = { currentVideoData->data() };
            const int dstStride[1] = { outWidth * 4 };
            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dst, dstStride);

            av_packet_unref(packet);
            return;
          }
        }
      }

      av_packet_unref(packet);
    }
  }

  void MediaParser::update()
  {
    const float fixedUpdateDt = 1.0f / static_cast<float>(getFrameRate());
    const auto currentTime = std::chrono::steady_clock::now();
    const float dt = std::chrono::duration<float>(currentTime - previousTime).count();
    previousTime = currentTime;
    timeAccumulator += dt;

    if (state == MediaState::AUTO_PLAYING)
    {
      while (timeAccumulator >= fixedUpdateDt)
      {
        loadNextFrame();

        timeAccumulator -= fixedUpdateDt;
      }
    }
    else if (state == MediaState::PAUSED || state == MediaState::MANUAL)
    {
      while (timeAccumulator >= fixedUpdateDt)
      {
        timeAccumulator -= fixedUpdateDt;
      }
    }
  }

  void MediaParser::play()
  {
    state = MediaState::AUTO_PLAYING;
  }

  void MediaParser::pause()
  {
    state = MediaState::PAUSED;
  }

  void MediaParser::setManual(const bool manual)
  {
    state = manual ? MediaState::MANUAL : MediaState::AUTO_PLAYING;
  }

  MediaState MediaParser::getState() const
  {
    return state;
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