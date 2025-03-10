#include "AudioPlayer.h"
#include <SDL3/SDL_init.h>
#include <stdexcept>

namespace Audio {
  AudioPlayer::AudioPlayer(const AudioParams& params)
    : params(params)
  {
    initSDL();

    configureAudioSpec(params);

    setupAudioComponents();
  }

  AudioPlayer::~AudioPlayer()
  {
    if (components.audioDevice)
    {
      SDL_PauseAudioDevice(components.audioDevice);
      SDL_CloseAudioDevice(components.audioDevice);
    }

    if (components.audioStream)
    {
      SDL_DestroyAudioStream(components.audioStream);
    }

    SDL_Quit();
  }

  void AudioPlayer::start() const
  {
    SDL_ResumeAudioDevice(components.audioDevice);
  }

  void AudioPlayer::stop() const
  {
    SDL_PauseAudioDevice(components.audioDevice);
  }

  int AudioPlayer::getAvailableBuffer() const
  {
    return SDL_GetAudioStreamAvailable(components.audioStream);
  }

  void AudioPlayer::queueAudio(const uint8_t* buffer, const int bufferSize) const
  {
    if (SDL_PutAudioStreamData(components.audioStream, buffer, bufferSize) < 0)
    {
      throw std::runtime_error("Failed to queue audio data");
    }
  }

  void AudioPlayer::setVolume(const float volume) const
  {
    SDL_SetAudioStreamGain(components.audioStream, volume);
  }

  void AudioPlayer::initSDL()
  {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0)
    {
      throw std::runtime_error("Failed to initialize SDL");
    }
  }

  void AudioPlayer::configureAudioSpec(const AudioParams& params)
  {
    SDL_zero(audioSpec);
    audioSpec.freq = params.sampleRate;
    audioSpec.channels = params.channels;

    // Determine format based on bits per sample
    if (params.bitsPerSample == 8)
    {
      audioSpec.format = SDL_AUDIO_U8;
    }
    else if (params.bitsPerSample == 16)
    {
      audioSpec.format = SDL_AUDIO_S16;
    }
    else if (params.bitsPerSample == 32)
    {
      audioSpec.format = SDL_AUDIO_S32;
    }
    else
    {
      SDL_Quit();
      throw std::runtime_error("Unsupported bits Per Sample");
    }
  }

  void AudioPlayer::setupAudioComponents()
  {
    // Create audio stream
    components.audioSpec = audioSpec;
    components.audioStream = SDL_CreateAudioStream(&audioSpec, &components.audioSpec);
    if (!components.audioStream)
    {
      SDL_Quit();
      throw std::runtime_error("Failed to create audio stream");
    }

    // Open audio device
    components.audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    if (components.audioDevice == 0)
    {
      SDL_DestroyAudioStream(components.audioStream);
      SDL_Quit();
      throw std::runtime_error("Failed to open audio device");
    }

    // Bind the audio stream to the device
    if (SDL_BindAudioStream(components.audioDevice, components.audioStream) < 0)
    {
      SDL_CloseAudioDevice(components.audioDevice);
      SDL_DestroyAudioStream(components.audioStream);
      SDL_Quit();
      throw std::runtime_error("Failed to bind audio stream to device");
    }
  }
} // Audio