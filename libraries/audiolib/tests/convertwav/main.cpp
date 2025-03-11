#include <audio.h>

int main(const int argc, char* argv[]) {
  Audio::convertWav(argc == 2 ? argv[1] : "audio.mp3", "audio");
}