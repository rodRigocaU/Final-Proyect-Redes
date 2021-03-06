#ifndef COLORS_HPP_
#define COLORS_HPP_

#define NO_COLOR "\e[0m"
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"
#define RED_BG "\e[41m"
#define VIOLET "\e[35m"

#include <iostream>

namespace tool
{

  void ConsolePrint(const char* message, const char* STYLE_CONFIG);

}

#endif//COLORS_HPP_