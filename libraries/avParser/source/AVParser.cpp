#include "AVParser.h"
extern "C" {
#include <libavutil/opt.h>
}
#include <stdexcept>
#include <ranges>
#include <thread>

namespace AVParser {
  MediaParser::MediaParser(const std::string& mediaFile, const AudioParams& params)
    : currentFrame(0), currentVideoData(std::make_shared<std::vector<uint8_t>>()),
      backgroundVideoData(std::make_shared<std::vector<uint8_t>>()),
      currentAudioData(std::make_shared<std::vector<uint8_t>>()), previousTime(std::chrono::steady_clock::now()),
      params(params)
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

    loadKeyframes();

    calculateTotalFrames();

    frame = av_frame_alloc();
    packet = av_packet_alloc();

    backgroundThread = std::thread(&MediaParser::backgroundFrameLoader, this);

    loadNextFrame();
  }

  MediaParser::~MediaParser()
  {
    keepLoadingInBackground = false;
    backgroundThread.join();

    av_packet_free(&packet);
    av_frame_free(&frame);

    swr_free(&swrContext);
    avcodec_free_context(&audioCodecContext);

    sws_freeContext(swsContext);
    avcodec_free_context(&videoCodecContext);

    avformat_close_input(&formatContext);
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

    const auto fps = AVRational{
      formatContext->streams[videoStreamIndex]->avg_frame_rate.num,
      formatContext->streams[videoStreamIndex]->avg_frame_rate.den
    };

    return av_q2d(fps);
  }

  uint32_t MediaParser::getTotalFrames() const
  {
    return totalFrames;
  }

  uint32_t MediaParser::getCurrentFrameIndex() const
  {
    return currentFrame;
  }

  void MediaParser::loadNextFrame()
  {
    if (currentFrame + 1 > getTotalFrames())
    {
      state = MediaState::PAUSED;
      return;
    }

    loadFrameFromCache(currentFrame + 1);

    currentFrame++;
  }

  void MediaParser::loadPreviousFrame()
  {
    if (currentFrame <= 0)
    {
      return;
    }

    loadFrameFromCache(currentFrame - 1);

    currentFrame--;
  }

  void MediaParser::loadFrameAt(const uint32_t targetFrame)
  {
    if (targetFrame > getTotalFrames())
    {
      throw std::out_of_range("Target frame is out of range!");
    }

    state = MediaState::PAUSED;

    currentFrame = targetFrame;

    loadFrameFromCache(targetFrame);

    // Calculate the correct audio chunk based on frame rate and target frame
    const double frameRate = getFrameRate();
    const AVStream* audioStream = formatContext->streams[audioStreamIndex];
    const double audioPtsPerSecond = 1.0 / av_q2d(audioStream->time_base);

    // Calculate the timestamp in seconds for the target frame
    const double targetTimeInSeconds = static_cast<double>(targetFrame) / frameRate;

    // Convert time to audio PTS
    const auto targetAudioPts = static_cast<int64_t>(targetTimeInSeconds * audioPtsPerSecond);

    // Find the closest audio chunk to this PTS
    auto it = audioCache.lower_bound(targetAudioPts);

    // If we found an exact match or a later chunk
    if (it != audioCache.end())
    {
      // Calculate the index (distance from beginning)
      currentAudioChunk = std::distance(audioCache.begin(), it);
    }
    else if (!audioCache.empty())
    {
      // If no exact match found, use the last available chunk
      currentAudioChunk = audioCache.size() - 1;
    }
    else
    {
      // No audio chunks available
      currentAudioChunk = 0;
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
      timeAccumulator = std::fmod(timeAccumulator, fixedUpdateDt);
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

  bool MediaParser::getNextAudioChunk(uint8_t*& outBuffer, int& outBufferSize)
  {
    if (currentAudioChunk >= audioCache.size())
    {
      return false;
    }

    const auto it = std::next(audioCache.begin(), currentAudioChunk); // Efficient lookup

    if (it == audioCache.end())
    {
      return false;
    }

    outBuffer = it->second.data();
    outBufferSize = static_cast<int>(it->second.size());

    currentAudioChunk++;

    return true;
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
  }

  void MediaParser::loadKeyframes()
  {
    const AVStream* videoStream = formatContext->streams[videoStreamIndex];

    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    // Read packets and store keyframe positions
    AVPacket packet;
    bool firstKeyframeFound = false;
    int64_t firstKeyframePts = 0;

    // First pass: find the first keyframe's PTS
    while (!firstKeyframeFound && av_read_frame(formatContext, &packet) >= 0)
    {
      if (packet.stream_index == videoStreamIndex && packet.flags & AV_PKT_FLAG_KEY)
      {
        firstKeyframePts = packet.pts;
        firstKeyframeFound = true;
      }
      av_packet_unref(&packet);
    }

    /* Find Video Keyframes */
    // Reset to beginning
    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    while (av_read_frame(formatContext, &packet) >= 0)
    {
      if (packet.stream_index == videoStreamIndex && packet.flags & AV_PKT_FLAG_KEY)
      {
        // Calculate frame number relative to the first keyframe
        const int64_t pts = packet.pts;
        const int64_t ptsDiff = pts - firstKeyframePts;

        const double frameDuration = av_q2d(AVRational{
          videoStream->avg_frame_rate.den,
          videoStream->avg_frame_rate.num
        });

        int frameNumber = static_cast<int>(static_cast<double>(ptsDiff) * av_q2d(videoStream->time_base) / frameDuration);

        // The first keyframe becomes frame 0
        keyFrameMap[frameNumber] = true;
      }
      av_packet_unref(&packet);
    }

    /* Find Audio Keyframes */
    // Reset to beginning
    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);

    auto keyFrame = keyFrameMap.begin();

    while (av_read_frame(formatContext, &packet) >= 0)
    {
      if (packet.stream_index == videoStreamIndex && packet.flags & AV_PKT_FLAG_KEY)
      {
        keyFrame->second = static_cast<int>(packet.pts);

        ++keyFrame;
      }
      av_packet_unref(&packet);
    }

    // Reset stream position
    av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
  }

  void MediaParser::calculateTotalFrames()
  {
    validateVideoStream();

    const AVStream* stream = formatContext->streams[videoStreamIndex];

    if (stream->nb_frames > 0)
    {
      totalFrames = stream->nb_frames;
    }

    // Otherwise calculate based on duration and frame rate
    if (stream->duration != AV_NOPTS_VALUE)
    {
      const double frameRate = getFrameRate();
      const double durationSeconds = static_cast<double>(stream->duration) * av_q2d(stream->time_base);
      totalFrames = static_cast<int>(durationSeconds * frameRate);
    }

    // If metadata methods fail, count frames more efficiently by seeking to last keyframe
    AVFormatContext* tempFormatCtx = nullptr;
    if (avformat_open_input(&tempFormatCtx, formatContext->url, nullptr, nullptr) < 0)
    {
      throw std::runtime_error("Failed to open video file for frame counting!");
    }

    if (avformat_find_stream_info(tempFormatCtx, nullptr) < 0)
    {
      avformat_close_input(&tempFormatCtx);
      throw std::runtime_error("Failed to retrieve stream info for frame counting!");
    }

    // Find video stream index in temp context
    int tempVideoStreamIndex = -1;
    for (size_t i = 0; i < tempFormatCtx->nb_streams; i++)
    {
      if (tempFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
      {
        tempVideoStreamIndex = static_cast<int>(i);
        break;
      }
    }

    if (tempVideoStreamIndex == -1)
    {
      avformat_close_input(&tempFormatCtx);
      throw std::runtime_error("No video stream found for frame counting!");
    }

    // stream
    const auto videoStream = tempFormatCtx->streams[tempVideoStreamIndex];

    // Set up decoder for the video stream
    const AVCodec* tempCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    AVCodecContext* tempCodecCtx = avcodec_alloc_context3(tempCodec);
    avcodec_parameters_to_context(tempCodecCtx, videoStream->codecpar);

    if (avcodec_open2(tempCodecCtx, tempCodec, nullptr) < 0)
    {
      avcodec_free_context(&tempCodecCtx);
      avformat_close_input(&tempFormatCtx);
      throw std::runtime_error("Failed to open codec for frame counting!");
    }

    // If we have keyframes, seek to the last known keyframe
    int frameCount = 0;
    if (!keyFrameMap.empty())
    {
      // Get the last keyframe position
      const int lastKeyframe = keyFrameMap.rbegin()->first;
      frameCount = lastKeyframe;

      // Calculate PTS for the last keyframe
      const int64_t targetPts = av_rescale_q(lastKeyframe,
                                             AVRational{videoStream->avg_frame_rate.den, videoStream->avg_frame_rate.num},
                                             videoStream->time_base) + videoStream->start_time;

      // Seek to the last keyframe
      if (av_seek_frame(tempFormatCtx, tempVideoStreamIndex, targetPts, AVSEEK_FLAG_BACKWARD) < 0)
      {
        avcodec_free_context(&tempCodecCtx);
        avformat_close_input(&tempFormatCtx);
        totalFrames = lastKeyframe; // Return last keyframe number if seek fails
      }

      // Flush buffers after seeking
      avcodec_flush_buffers(tempCodecCtx);
    }

    // Allocate packet and frame
    AVPacket* tempPacket = av_packet_alloc();
    AVFrame* tempFrame = av_frame_alloc();

    // Count additional frames after the last keyframe
    int additionalFrames = 0;
    while (av_read_frame(tempFormatCtx, tempPacket) >= 0)
    {
      if (tempPacket->stream_index == tempVideoStreamIndex)
      {
        if (avcodec_send_packet(tempCodecCtx, tempPacket) == 0)
        {
          while (avcodec_receive_frame(tempCodecCtx, tempFrame) == 0)
          {
            additionalFrames++;
          }
        }
      }
      av_packet_unref(tempPacket);
    }

    // Add additional frames to the count
    frameCount += additionalFrames;

    // Clean up
    av_frame_free(&tempFrame);
    av_packet_free(&tempPacket);
    avcodec_free_context(&tempCodecCtx);
    avformat_close_input(&tempFormatCtx);

    totalFrames = frameCount;
  }

  void MediaParser::setupAudio()
  {
    // Get codec parameters
    const AVCodecParameters* codecParams = formatContext->streams[audioStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec)
    {
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to find decoder");
    }

    // Allocate codec context
    audioCodecContext = avcodec_alloc_context3(codec);
    if (!audioCodecContext)
    {
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to allocate codec context");
    }

    // Copy codec parameters to context
    if (avcodec_parameters_to_context(audioCodecContext, codecParams) < 0)
    {
      avcodec_free_context(&audioCodecContext);
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to copy codec parameters to context");
    }

    // Open codec
    if (avcodec_open2(audioCodecContext, codec, nullptr) < 0)
    {
      avcodec_free_context(&audioCodecContext);
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to open codec");
    }

    // Create resampler context
    swrContext = swr_alloc();
    if (!swrContext)
    {
      avcodec_free_context(&audioCodecContext);
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to allocate SwrContext");
    }

    // Set input options
    av_opt_set_chlayout(swrContext, "in_chlayout", &audioCodecContext->ch_layout, 0);
    av_opt_set_int(swrContext, "in_sample_rate", audioCodecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", audioCodecContext->sample_fmt, 0);

    // Set output options
    constexpr AVChannelLayout outLayout = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(swrContext, "out_chlayout", &outLayout, 0);
    av_opt_set_int(swrContext, "out_sample_rate", params.sampleRate, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    // Initialize SwrContext
    if (swr_init(swrContext) < 0)
    {
      swr_free(&swrContext);
      avcodec_free_context(&audioCodecContext);
      avformat_close_input(&formatContext);
      throw std::runtime_error("Failed to initialize SwrContext");
    }
  }

  void MediaParser::validateVideoContext() const
  {
    if (!videoCodecContext)
    {
      throw std::runtime_error("Video codec context not initialized!");
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
    const int64_t targetPts = av_rescale_q(targetFrame,
                              AVRational{stream->avg_frame_rate.den, stream->avg_frame_rate.num},
                              stream->time_base) + stream->start_time;

    // Seek to the nearest keyframe before the target
    if (av_seek_frame(formatContext, videoStreamIndex, targetPts, AVSEEK_FLAG_BACKWARD) < 0)
    {
      throw std::runtime_error("Seek failed");
    }

    // Flush the video decoder to clear internal buffers
    avcodec_flush_buffers(videoCodecContext);
    avcodec_flush_buffers(audioCodecContext);
  }

  void MediaParser::loadFrame()
  {
    if (frame == nullptr)
    {
      throw std::runtime_error("No video frame found!");
    }

    if (packet == nullptr)
    {
      throw std::runtime_error("No video packet found!");
    }

    while (av_read_frame(formatContext, packet) >= 0)
    {
      if (packet->stream_index == videoStreamIndex)
      {
        if (avcodec_send_packet(videoCodecContext, packet) == 0)
        {
          while (avcodec_receive_frame(videoCodecContext, frame) == 0)
          {
            convertVideoFrame();
            av_packet_unref(packet);
            return;
          }
        }
      }
      av_packet_unref(packet);
    }
  }

  void MediaParser::convertVideoFrame() const
  {
    if (!frame->data[0])
    {
      throw std::runtime_error("Invalid frame data in convertVideoFrame");
    }

    const int outWidth = getFrameWidth();
    const int outHeight = getFrameHeight();

    // Resize or reallocate the buffer if needed
    if (backgroundVideoData->size() != outWidth * outHeight * 4)
    {
      backgroundVideoData->resize(outWidth * outHeight * 4);
    }

    uint8_t* dst[1] = { backgroundVideoData->data() };
    const int dstStride[1] = { outWidth * 4 };

    if (!swsContext)
    {
      throw std::runtime_error("Invalid swsContext in convertVideoFrame");
    }

    sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dst, dstStride);
  }

  void MediaParser::loadFrameFromCache(const uint32_t targetFrame)
  {
    bool found = false;
    std::vector<unsigned char> data;

    while (!found)
    {
      auto it = keyFrameMap.upper_bound(static_cast<int>(targetFrame));
      if (it == keyFrameMap.begin())
      {
        throw std::runtime_error("Key frame not found!");
      }
      --it;
      const auto targetKeyFrame = it->first;

      auto keyFrameIt = videoCache.find(targetKeyFrame);
      if (keyFrameIt == videoCache.end())
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      // Access the keyframe map from the cache
      const auto& frames = keyFrameIt->second;
      const auto relativeFrame = targetFrame - targetKeyFrame;
      if (relativeFrame > frames.size())
      {
        throw std::runtime_error("Target frame not found in key frame.");
      }

      data = frames.at(relativeFrame);
      found = true;
    }

    // Set currentVideoData to the frame
    currentVideoData = std::make_shared<std::vector<uint8_t>>(data);
  }

  void MediaParser::loadFrames(const uint32_t targetFrame)
  {
    auto it = keyFrameMap.upper_bound(static_cast<int>(targetFrame));
    if (it == keyFrameMap.begin())
    {
      throw std::runtime_error("Key frame not found!");
    }
    const auto nextKeyFrame = it == keyFrameMap.end() ? getTotalFrames() + 1 : it->first;
    --it;
    const auto targetKeyFrame = it->first;

    FrameCache frameCache;
    frameCache.reserve(nextKeyFrame - targetKeyFrame);

    seekToFrame(targetKeyFrame);

    for (uint32_t i = targetKeyFrame; i < nextKeyFrame; ++i)
    {
      loadFrame();
      frameCache.push_back(*backgroundVideoData);
    }

    videoCache[targetKeyFrame] = std::move(frameCache);

    seekToFrame(targetKeyFrame);

    for (uint32_t i = targetKeyFrame; i < nextKeyFrame * 2; ++i)
    {
      try
      {
        uint8_t* buffer = nullptr;
        int bufferSize = 0;
        decodeAudioChunk(buffer, bufferSize);
        av_freep(&buffer);
      }
      catch ([[maybe_unused]] const std::exception& e)
      { /* Some frames may not load but that's expected and OKAY. */ }
    }
  }

  bool MediaParser::decodeAudioChunk(uint8_t*& outBuffer, int& outBufferSize)
  {
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    bool gotAudio = false;
    bool endOfFile = false;

    outBuffer = nullptr;
    outBufferSize = 0;

    if (!packet || !frame)
    {
      if (packet)
      {
        av_packet_free(&packet);
      }
      if (frame)
      {
        av_frame_free(&frame);
      }
      throw std::runtime_error("Failed to allocate packet or frame");
    }

    while (!gotAudio && !endOfFile)
    {
      int readResult = av_read_frame(formatContext, packet);

      if (readResult >= 0)
      {
        if (packet->stream_index == audioStreamIndex)
        {
          if (avcodec_send_packet(audioCodecContext, packet) < 0)
          {
            throw std::runtime_error("Failed to send packet for decoding");
          }

          const int receiveResult = avcodec_receive_frame(audioCodecContext, frame);
          if (receiveResult == AVERROR(EAGAIN) || receiveResult == AVERROR_EOF)
          {
            break;
          }

          if (receiveResult < 0)
          {
            throw std::runtime_error("Error during decoding");
          }

          // Calculate output buffer size
          const int out_samples = static_cast<int>(av_rescale_rnd(
              swr_get_delay(swrContext, audioCodecContext->sample_rate) + frame->nb_samples,
              params.sampleRate,
              audioCodecContext->sample_rate,
              AV_ROUND_UP
          ));

          if (outBuffer)
          {
            av_freep(&outBuffer);
            outBuffer = nullptr;
          }

          // Allocate output buffer
          const int buffer_size = av_samples_alloc(&outBuffer, nullptr, params.channels,
                                                   out_samples, AV_SAMPLE_FMT_S16, 0);

          if (buffer_size < 0)
          {
            throw std::runtime_error("Failed to allocate samples buffer");
          }

          // Convert audio samples
          const int samples_converted = swr_convert(
              swrContext,
              &outBuffer, out_samples,
              frame->data, frame->nb_samples
          );

          if (samples_converted > 0)
          {
            const int bytesPerSample = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            outBufferSize = samples_converted * params.channels * bytesPerSample;
            gotAudio = true;

            // Cache
            audioCache[frame->pts] = std::vector(outBuffer, outBuffer + outBufferSize);

            break;
          }

          av_freep(&outBuffer);
        }
        av_packet_unref(packet);
      }
      else if (readResult == AVERROR_EOF)
      {
        // End of file, send null packet to flush decoder
        avcodec_send_packet(audioCodecContext, nullptr);
        endOfFile = true;
      }
      else
      {
        av_frame_free(&frame);
        av_packet_free(&packet);
        if (outBuffer)
        {
          av_freep(&outBuffer);
        }
        throw std::runtime_error("Failed to decode frame");
      }
    }

    av_frame_free(&frame);
    av_packet_free(&packet);

    if (!gotAudio && outBuffer)
    {
      av_freep(&outBuffer);
      outBuffer = nullptr;
    }

    return gotAudio;
  }

  void MediaParser::backgroundFrameLoader()
  {
    while (keepLoadingInBackground)
    {
      // Get current frame and playback state
      const uint32_t currentFrameIdx = currentFrame;
      const MediaState currentState = state;

      // Determine which keyframes to load based on playback direction
      auto it = keyFrameMap.upper_bound(static_cast<int>(currentFrameIdx));
      if (it == keyFrameMap.begin())
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      --it;

      // Load current keyframe's frames if not cached
      if (const auto currentKeyFrame = it->first; !videoCache.contains(currentKeyFrame))
      {
        loadFrames(currentKeyFrame);
      }

      // Determine next frames to preload based on playback state
      if (currentState == MediaState::AUTO_PLAYING)
      {
        // Preload next keyframe for forward playback
        ++it;
        if (it != keyFrameMap.end()) {
          if (const auto nextKeyFrame = it->first; !videoCache.contains(nextKeyFrame))
          {
            loadFrames(nextKeyFrame);
          }
        }
      }
      else if (currentState == MediaState::MANUAL)
      {
        // For manual mode, preload both forward and backward
        // First the next keyframe
        auto tempIt = it;
        ++tempIt;
        if (tempIt != keyFrameMap.end())
        {
          if (const auto nextKeyFrame = tempIt->first; !videoCache.contains(nextKeyFrame))
          {
            loadFrames(nextKeyFrame);
          }
        }

        // Then the previous keyframe
        if (it != keyFrameMap.begin())
        {
          --it;
          if (const auto prevKeyFrame = it->first; !videoCache.contains(prevKeyFrame))
          {
            loadFrames(prevKeyFrame);
          }
        }
      }

      // Smarter cache management - keep keyframes around current position
      while (videoCache.size() > 4)
      {
        // Find the keyframe farthest from current position to remove
        uint32_t farthestKeyFrame = 0;
        int64_t maxDistance = -1;

        for (const auto& cache : videoCache)
        {
          const int64_t distance = std::abs(static_cast<int64_t>(cache.first) - static_cast<int64_t>(currentFrameIdx));
          if (distance > maxDistance)
          {
            maxDistance = distance;
            farthestKeyFrame = cache.first;
          }
        }

        if (maxDistance > 0)
        {
          const int chunks = videoCache.size();

          for (int i = 0; i < chunks; ++i)
          {
            if (farthestKeyFrame > currentFrame)
            {
              audioCache.erase(std::prev(audioCache.end()));
            }
            else
            {
              audioCache.erase(audioCache.begin());
              currentAudioChunk--;
            }
          }

          videoCache.erase(farthestKeyFrame);
        }
        else
        {
          break;
        }
      }
    }
  }

  void MediaParser::setFilepath(const std::string& mediaFile)
  {
    currentFrame = 0;
    currentVideoData = std::make_shared<std::vector<uint8_t>>();
    currentAudioData = std::make_shared<std::vector<uint8_t>>();
    previousTime = std::chrono::steady_clock::now();
    formatContext = nullptr;
    videoCache.clear();
    
    frame = nullptr;
    packet = nullptr;
    swsContext = nullptr;

    videoCodec = nullptr;
    videoCodecContext = nullptr;

    audioCodec = nullptr;
    audioCodecContext = nullptr;

    videoStreamIndex = -1;
    audioStreamIndex = -1;

    timeAccumulator = 0;

    state = MediaState::AUTO_PLAYING;

    keyFrameMap.clear();

    totalFrames = 0;

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

    loadKeyframes();

    calculateTotalFrames();

    frame = av_frame_alloc();
    packet = av_packet_alloc();

    loadNextFrame();
  }
} // AVParser