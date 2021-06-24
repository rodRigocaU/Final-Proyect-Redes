#include "App/Tools/Colors.hpp"

namespace tool{

  void ConsolePrint(const char* message, const char* STYLE_CONFIG) {
    std::cout << STYLE_CONFIG << message << NO_COLOR << std::endl;
  }

}