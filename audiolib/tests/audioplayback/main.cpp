#include <source/audio.h>

int main() 
{
  Audio::initSDL();

  // create and play audio stream
  Audio::AudioData audio = Audio::playAudio("audio.wav");
  uint32_t duration = audio.duration;
  uint32_t stop = audio.duration - 10000;

  while(duration > 0){
    Audio::delay(10); // checks for extra input every 10 ms
    
    // do any checks for other inputs
    if(duration == stop){
      break; //plays 10 seconds of audio
    }
    
    duration -= 10;
  }

  Audio::deleteStream(audio.stream);
  Audio::quitSDL();
}