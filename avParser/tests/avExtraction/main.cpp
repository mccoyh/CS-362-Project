#include <iostream>
#include <source/AVParser.h>

int main()
{
  try
  {
    const auto parser = AVParser::AVParser("assets/CS_test.mp4");

    const auto frameData = parser.getCurrentFrame();

    std::cout << "Frame Width: " << frameData.frameWidth << "\n"
              << "Frame Height: " << frameData.frameHeight << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}