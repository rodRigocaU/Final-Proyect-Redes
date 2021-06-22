#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <iostream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace tool{
  using Record = std::vector<std::string>;
  using Records = std::vector<Record>;

  void printMsgError(char *msg);

  void decoOperator(std::string opt);

  int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names);

  int insert_callback(void *NotUsed, int argc, char **argv, char **azColName);

  int print_select_callback(void *flag, int argc, char **argv, char **azColName);

}

#endif//TOOLS_HPP_