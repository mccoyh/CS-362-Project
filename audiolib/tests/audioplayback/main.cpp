#include <source/audio.h>

//using namespace Audio;

int main() 
{
  Audio::initSDL();

  // create and play audio stream
  Audio::AudioData audio = Audio::playAudio("audio.wav");
  uint32_t duration = audio.duration;

  while(duration > 0){
    Audio::delay(10); // checks for extra input every 10 ms
    
    // do any checks for other inputs
    
    duration -= 10;
  }

  Audio::deleteStream(audio.stream);
  Audio::quitSDL();
}