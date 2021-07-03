#ifndef REPOSITORY_0_MASTER_PROTOCOL_HPP_
#define REPOSITORY_0_MASTER_PROTOCOL_HPP_

#include <list>
#include <string>
#include <utility>

/*
  <l> + <IdNode> + | + <IdNode | &>
*/
#define COMMAND_LINK      'l'
/*
  <d> + <IdNode> + | + <IdNode | &>
*/
#define COMMAND_DETACH    'd'
/*
  <r> + <new_name>
*/
#define COMMAND_RENAME    'r'

#define COMMAND_SEPARATOR '|'
#define COMMAND_SPLIT     '&'

namespace tool{

  std::pair<uint64_t, std::list<uint64_t>> parseRepoActiveCommand(const std::string& commandBody);

}

#endif//REPOSITORY_0_MASTER_PROTOCOL_HPP_