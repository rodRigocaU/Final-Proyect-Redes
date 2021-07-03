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
#include "App/TransportParser/Create.hpp"
#include "App/TransportParser/Delete.hpp"
#include "App/TransportParser/Update.hpp"

namespace db
{

    class SQLite
    {
    private:
        std::string nameDatabase;
        // descriptor database
        sqlite3 *DB;
        int exit;

        char *MsgError;
        int rc;

        bool printRecord = true;
        std::string sql;

    public:
        bool printError = true;

        SQLite(const std::string &nameDatabase = "GraphNetwork.db");
        void createTables();
        //CRUD
        //C
        void Create(msg::CreateNodePacket &packetCreate);
        void CreateNodo(std::string &name_node, std::string &idNode);
        void CreateAttributes(std::string &idNode, std::map<std::string, std::string> &attributes);
        void CreateRelation(std::string &id_node_start, std::string &id_node_end);
        void CreateRelations(std::string &idNode, std::vector<std::string> &nodes_relations);

        //R (Falta)
        void Read(std::string &query_node, uint8_t deep, msg::ReadNodePacket::Class &leaf, msg::ReadNodePacket::QueryMode &attributes, std::vector<msg::ReadNodePacket::Feature> &features);

        // U
        void Update(msg::UpdateNodePacket &packetUpdate);
        void UpdateValueNodo(std::string &query_value_node, std::string &set_value_node);
        void UpdateAttribute(std::string &query_value_node, std::string &name_attribute, std::string &value_attribute);

        //D
        void Delete(msg::DeleteNodePacket &packetDelete);
        void DeleteNode(std::string &query_value_node);
        void DeleteValueAttribute(std::string &query_value_node, std::string &query_value_attribute);
        void DeleteRelation(std::string &query_value_node, std::string &query_Relation);

        void existDataBase();
        // bool exsitNodo(std::string name_node);
        bool exsitNodo(std::string name_node, std::string id_Node = "0");
        bool exsitNodo2(std::string name_node, std::string &id_Node);
        void closeDB();
        void cleanDB(bool wantNode = true, bool wantAttributes = true, bool wantRelations = true);

        // Print Select
        void printSelectNodos();
        void printSelectRelations();
        void printSelectAttributes();

        //foreign_keys -> Constraint Support
        void ConstraintForeign();
    };

}

#endif //SQLITE_HPP_