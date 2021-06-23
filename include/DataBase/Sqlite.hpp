#ifndef SQLITE_HPP_
#define SQLITE_HPP_

#include <iostream>
// #include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <map>

#include "DataBase/Tools.hpp"

namespace db
{

    class SQLite
    {
    private:
        std::string nameDatabase;
        // descriptor database
        sqlite3 *DB;
        int exit;

        char *MsgError = 0;
        int rc;

        bool printRecord = false;

        std::string sql;

    public:
        bool printError = true;

        SQLite(const std::string &nameDatabase = "GraphNetwork.db");

        //CRUD
        //C
        void Create(std::string name_node, std::map<std::string, std::string> attributes, std::vector<std::string> nodes_relations);
        //R (Falta)
        std::vector<std::string> Read(std::vector<std::string> path, std::string query_node, std::string deep, std::string leaf, std::string attributes, std::vector<std::tuple<std::string, std::string, std::string, std::string>> condition);

        // U
        void UpdateValueNodo(std::string query_value_node, std::string set_value_node);
        void UpdateAttribute(std::string query_value_node,std::string name_attribute,std::string value_attribute);
        void UpdateAttributes(std::string query_value_node, std::map<std::string, std::string> set_attributes);
        //D
        void DropNode(std::string query_value_node);
        void DropValueAttributeOrRelation(std::string query_value_node, std::string query_value_attribute_o_R_size, std::string node_or_attribute);

        void createTables();
        void insertNodo(std::string name_node);
        void insertAttributes(std::string name_node, std::map<std::string, std::string> attributes);
        void createRelation(std::string name_node_start, std::string name_node_end);
        void createRelations(std::string name_node_start, std::vector<std::string> nodes_relations);

        void existDataBase();
        bool exsitNodo(std::string name_node);
        void closeDB();

        // Print Select
        void printSelectNodos();
        void printSelectRelations();
        void printSelectAttributes();
    };

}

#endif //SQLITE_HPP_