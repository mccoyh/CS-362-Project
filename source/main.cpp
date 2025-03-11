#include "MediaPlayer.h"
#include <iostream>

int main(const int argc, char* argv[])
{
  try
  {
    MediaPlayer mediaPlayer{argc == 2 ? argv[1] : "assets/CS_test.mp4"};

    mediaPlayer.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}