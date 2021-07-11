#ifndef REPOSITORY_0_MASTER_PROTOCOL_HPP_
#define REPOSITORY_0_MASTER_PROTOCOL_HPP_

#include <list>
#include <string>

/*
  <?>
*/
#define COMMAND_LIST      '?'
/*
  <L> + <IdNode | &> -- <L> <targetId> + <targetLinkPort>
*/
#define COMMAND_LINK      'L'
/*
  <D> + <IdNode | &> -- <D> <targetId>
*/
#define COMMAND_DETACH    'D'
/*
  <R> + <new_name>
*/
#define COMMAND_RENAME    'R'
/*
  <K>
*/
#define COMMAND_KILL      'K'

#define COMMAND_SEPARATOR '|'
#define COMMAND_SPLIT     ','

#define QUERY_PORT(pair) pair.first.first
#define LINK_PORT(pair)  pair.first.second
#define GET_IP_ADDRESS(pair) pair.second

namespace tool{

  typedef uint64_t IdNetNode;
  typedef int32_t FileDescriptor;
  
  std::list<IdNetNode> parseRepoActiveCommand(const std::string& commandBody);

}

#endif//REPOSITORY_0_MASTER_PROTOCOL_HPP_