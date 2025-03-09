#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <SDL3/SDL_audio.h>

namespace Audio {

struct AudioParams {
  int sampleRate = 44100;
  int channels = 2;
  int bitsPerSample = 16;
  double frequency = 420.0; // Frequency in Hz
};

class AudioPlayer {
public:
  explicit AudioPlayer(const AudioParams& params);

  ~AudioPlayer();

  void start() const;

  void stop() const;

  [[nodiscard]] int getAvailableBuffer() const;

  void queueAudio(const uint8_t* buffer, int bufferSize) const;

private:
  struct AudioComponents {
    SDL_AudioDeviceID audioDevice = 0;
    SDL_AudioStream* audioStream = nullptr;
    SDL_AudioSpec audioSpec{};
  };

  AudioParams params{};
  AudioComponents components{};
  SDL_AudioSpec audioSpec{};

  static void initSDL();

  void configureAudioSpec(const AudioParams& params);

  void setupAudioComponents();
};

} // Audio

#endif //AUDIOPLAYER_H
