#include <audio.h>

int main(const int argc, char* argv[]) {
  Audio::initSDL();

  // create and play audio stream
  Audio::AudioData audio = Audio::playAudio(argc == 2 ? argv[1] : "audio.wav");
  uint32_t duration = audio.duration;
  uint32_t change = audio.duration - 10000;
  uint32_t stop = 0;

  while(duration > 0){
    Audio::delay(10); // checks for extra input every 10 ms
    
    // do any checks for other inputs
    if(duration == change){
      // plays 10 seconds of audio, then change to 2x speed
      // change duration to account for change in speed
      duration /= 2;
      Audio::changeSpeed(audio.stream, 2);

      stop = duration - 10000; // play another 10 seconds of audio after changing speed, then stop

    } else if(duration == stop){
      break; 
    }
    
    duration -= 10;
  }

  Audio::deleteStream(audio.stream);
  Audio::quitSDL();
}