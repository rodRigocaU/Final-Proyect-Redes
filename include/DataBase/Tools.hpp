#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <iostream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sqlite3.h>
#include<map>
#include <iomanip>

namespace tool
{
  using Record = std::vector<std::string>;
  using Records = std::vector<Record>;

  const char separator = ' ';
  const int nameWidth = 15;
  const int numWidth = 15;

  void printMsgError(char *msg);

  void decoOperator(std::string opt);

  int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names);

  int insert_callback(void *NotUsed, int argc, char **argv, char **azColName);

  int print_select_callback(void *flag, int argc, char **argv, char **azColName);

  int select_relations_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names);

  void printNode(std::string &name_node);
  void printRecords(Records records);

  void saveAttributes(Records records,std::map<std::string,std::string> &attributes);

  bool isTxt(std::string nameAttribute);
  bool isMultimedia(std::string nameAttribute);

  template <typename T>
  void printElement(T t, const int &width);

}

#endif //TOOLS_HPP_