#ifndef SQLITE_HPP_
#define SQLITE_HPP_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <map>
#include "DataBase/Tools.hpp" //// #include <sqlite3.h>
#include "App/TransportParser/Read.hpp"

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

        bool printRecord = true;
        std::string sql;

    public:
        bool printError = true;

        SQLite(const std::string &nameDatabase);

        //CRUD
        //C
        void Create(std::string name_node, std::map<std::string, std::string> attributes, std::vector<std::string> nodes_relations);
        //R (Falta)
        void Read(std::string &query_node, uint8_t deep, msg::ReadNodePacket::Class &leaf, msg::ReadNodePacket::QueryMode &attributes, std::vector<msg::ReadNodePacket::Feature> &features);

        // U
        void UpdateValueNodo(std::string query_value_node, std::string set_value_node);
        void UpdateAttribute(std::string query_value_node, std::string name_attribute, std::string value_attribute);

        void UpdateAttributes(std::string query_value_node, std::map<std::string, std::string> set_attributes);
        //D
        void DropNode(std::string query_value_node);
        void DropValueAttribute(std::string query_value_node, std::string query_value_attribute);
        void DropRelation(std::string query_value_node, std::string query_Relation);

        void createTables();
        void insertNodo(std::string name_node);
        void insertAttributes(std::string name_node, std::map<std::string, std::string> attributes);
        void createRelation(std::string name_node_start, std::string name_node_end);
        void createRelations(std::string name_node_start, std::vector<std::string> nodes_relations);

        void existDataBase();
        bool exsitNodo(std::string name_node);
        void closeDB();
        void cleanDB(bool wantNode = true, bool wantAttributes = true, bool wantRelations = true);

        // Print Select
        void printSelectNodos();
        void printSelectRelations();
        void printSelectAttributes();
    };

}

#endif //SQLITE_HPP_