#ifndef READ_HPP_
#define READ_HPP_

#include "IncludeH.hpp"

namespace msg{

  struct ReadNodePacket {
    enum Class{Leaf, Internal, NoneClass};
    enum BooleanOperator{And, Or, NoneBO};
    enum QueryMode{Required, NotRequired, NoneQM};
    enum SqlOperator{Equal, LessThan, MoreThan, Like, NoneSQL};

    const std::unordered_map<SqlOperator, std::string> sqlOperators  = {{SqlOperator::Equal, "="},
                                                                        {SqlOperator::LessThan, "<"},
                                                                        {SqlOperator::MoreThan, ">"},
                                                                        {SqlOperator::Like, "like"}};
    const std::unordered_map<std::string, SqlOperator> sqlMeanings   = {{"=", SqlOperator::Equal},
                                                                        {"<", SqlOperator::LessThan},
                                                                        {">", SqlOperator::MoreThan},
                                                                        {"like", SqlOperator::Like}};

    struct Feature {
      std::string attrName, attrValue;
      SqlOperator sqlOpId = SqlOperator::NoneSQL;
      BooleanOperator boolOpId = BooleanOperator::NoneBO;

      void reset();
    };

    std::string nodeId;
    uint16_t depth;
    Class nodeType;
    QueryMode attribsReq;
    std::vector<Feature> features;

    ReadNodePacket();

    BooleanOperator toBooleanEnum(const std::string& boolOp);
    
    void clear();
  };

  ReadNodePacket& operator<<(ReadNodePacket& packet, const std::string& message);
  ReadNodePacket& operator>>(ReadNodePacket& packet, std::string& message);
  ReadNodePacket& operator<<(ReadNodePacket& packet, std::map<std::string, std::string>& settings);

}

#endif//READ_HPP_