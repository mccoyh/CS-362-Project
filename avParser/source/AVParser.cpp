#include "AVParser.h"
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
    validateVideoStream();

    const AVRational fps = formatContext->streams[videoStreamIndex]->r_frame_rate;

    return av_q2d(fps);
  }

  void MediaParser::loadNextFrame()
  {
    if (keyFrameMap.upper_bound(currentFrame + 1) == keyFrameMap.end())
    {
      return;
    }

    useCachedFrame(currentFrame + 1);

    currentFrame++;
  }

  void MediaParser::loadPreviousFrame()
  {
    if (currentFrame <= 0)
    {
      return;
    }

    useCachedFrame(currentFrame - 1);

    currentFrame--;
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

    validateVideoStream();

    validateAudioStream();
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

    loadVideoKeyframes();
  }

  void MediaParser::loadVideoKeyframes()
  {
    const AVStream* videoStream = formatContext->streams[videoStreamIndex];

    // Read packets and store keyframe positions
    AVPacket packet;
    while (av_read_frame(formatContext, &packet) >= 0)
    {
      if (packet.stream_index == videoStreamIndex)
      {
        if (packet.flags & AV_PKT_FLAG_KEY)
        {
          // Convert PTS to frame number
          int frameNumber = static_cast<int>(av_rescale_q(packet.pts, videoStream->time_base, AVRational{1, videoStream->avg_frame_rate.num}));

          keyFrameMap[frameNumber] = true;
        }
      }
      av_packet_unref(&packet);
    }
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

  void MediaParser::validateVideoStream() const
  {
    if (videoStreamIndex == -1)
    {
      throw std::runtime_error("No video stream found!");
    }
  }

  void MediaParser::validateAudioStream() const
  {
    if (audioStreamIndex == -1)
    {
      throw std::runtime_error("No audio stream found!");
    }
  }

  void MediaParser::seekToFrame(const int64_t targetFrame) const
  {
    validateVideoStream();

    const AVStream* stream = formatContext->streams[videoStreamIndex];

    // Calculate timestamp for the target frame
    const int64_t targetPts = av_rescale_q(targetFrame, av_inv_q(stream->r_frame_rate), stream->time_base)
                              + stream->start_time;

    // Seek to the nearest keyframe before the target
    if (av_seek_frame(formatContext, videoStreamIndex, targetPts, AVSEEK_FLAG_BACKWARD) < 0)
    {
      throw std::runtime_error("Seek failed");
    }

    // Flush the video decoder to clear internal buffers
    avcodec_flush_buffers(videoCodecContext);
  }

  void MediaParser::loadFrame(const uint32_t targetFrame) const
  {
    if (frame == nullptr)
    {
      throw std::runtime_error("No video frame found!");
    }

    if (packet == nullptr)
    {
      throw std::runtime_error("No video packet found!");
    }

    const AVStream* stream = formatContext->streams[videoStreamIndex];
    const AVRational frameDuration = av_inv_q(stream->avg_frame_rate);
    const int64_t targetPts = av_rescale_q(targetFrame, frameDuration, stream->time_base);

    while (av_read_frame(formatContext, packet) >= 0)
    {
      if (packet->stream_index == videoStreamIndex)
      {
        if (avcodec_send_packet(videoCodecContext, packet) == 0)
        {
          while (avcodec_receive_frame(videoCodecContext, frame) == 0)
          {
            if (frame->pts >= targetPts)
            {
              convertVideoFrame();
              av_packet_unref(packet);
              return;
            }
          }
        }
      }
      av_packet_unref(packet);
    }
  }

  void MediaParser::convertVideoFrame() const
  {
    if (!frame || !frame->data[0])
    {
      throw std::runtime_error("Invalid frame data in convertVideoFrame");
    }

    const int outWidth = getFrameWidth();
    const int outHeight = getFrameHeight();

    // Resize or reallocate the buffer if needed
    if (currentVideoData->size() != outWidth * outHeight * 4)
    {
      currentVideoData->resize(outWidth * outHeight * 4);
    }

    uint8_t* dst[1] = { currentVideoData->data() };
    const int dstStride[1] = { outWidth * 4 };

    if (!swsContext)
    {
      throw std::runtime_error("Invalid swsContext in convertVideoFrame");
    }

    sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dst, dstStride);
  }

  void MediaParser::useCachedFrame(const uint32_t targetFrame)
  {
    auto it = keyFrameMap.upper_bound(targetFrame);
    if (it == keyFrameMap.begin())
    {
      throw std::runtime_error("Key frame not found!");
    }
    --it;
    const auto targetKeyFrame = it->first;

    auto keyFrameIt = cache.find(targetKeyFrame);
    if (keyFrameIt == cache.end())
    {
      loadFrames(targetFrame);  // Load frames if not found
      keyFrameIt = cache.find(targetKeyFrame);  // Re-check after loading
    }

    // Access the keyframe map from the cache
    const auto& keyFrame = keyFrameIt->second;
    const auto frameIt = keyFrame.find(targetFrame);
    if (frameIt == keyFrame.end())
    {
      throw std::runtime_error("Target frame not found in key frame.");
    }

    // Set currentVideoData to the frame
    currentVideoData = std::make_shared<std::vector<uint8_t>>(frameIt->second);
  }

  void MediaParser::loadFrames(const uint32_t targetFrame)
  {
    auto it = keyFrameMap.upper_bound(targetFrame);
    if (it == keyFrameMap.begin() || it == keyFrameMap.end())
    {
      throw std::runtime_error("Key frame not found!");
    }
    const auto nextKeyFrame = it->first;
    --it;
    const auto targetKeyFrame = it->first;

    cache[targetKeyFrame] = {};

    auto& keyFrame = cache[targetKeyFrame];

    seekToFrame(targetKeyFrame);

    for (size_t i = targetKeyFrame; i < nextKeyFrame; ++i)
    {
      loadFrame(i);
      keyFrame[i] = *currentVideoData;
    }
  }
} // AVParser