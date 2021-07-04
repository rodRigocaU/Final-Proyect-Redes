#ifndef REPOSITORY_0_MASTER_PROTOCOL_HPP_
#define REPOSITORY_0_MASTER_PROTOCOL_HPP_

#include <list>
#include <string>

/*
  <?>
*/
#define COMMAND_LIST      '?'
/*
  <l> + <IdNode | &>
*/
#define COMMAND_LINK      'l'
/*
  <d> + <IdNode | &>
*/
#define COMMAND_DETACH    'd'
/*
  <r> + <new_name>
*/
#define COMMAND_RENAME    'r'
/*
  <k>
*/
#define COMMAND_KILL      'k'

#define COMMAND_SEPARATOR '|'
#define COMMAND_SPLIT     '&'

namespace tool{

  std::list<uint64_t> parseRepoActiveCommand(const std::string& commandBody);

}

#endif//REPOSITORY_0_MASTER_PROTOCOL_HPP_