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
#include "Tools.hpp" //// #include <sqlite3.h>
#include "App/TransportParser/Read.hpp"
#include "App/TransportParser/Create.hpp"
#include "App/TransportParser/Delete.hpp"
#include "App/TransportParser/Update.hpp"

namespace db
{

    class SQLite
    {
    private:
        //std::string nameDatabase;
        std::string nameDatabase="GraphNetwork.db";
        sqlite3 *DB; // descriptor database
        bool printError = true;
        char *MsgError;
        int rc; //Status Query
        std::string sql;

    public:
        void setFile(const std::string& file);
        void createTables();

        void existDataBase();
        bool existNodo(std::string name_node, std::string &id_Node);
        void closeDB();
        void cleanDB(bool wantNode = true, bool wantAttributes = true, bool wantRelations = true);
        //foreign_keys -> Constraint Support
        void ConstraintForeign();
        void writeComandSQL();

        void printSelectNodos();
        void printSelectRelations();
        void printSelectAttributes();

        //CRUD
        //C
        void Create(msg::CreateNodePacket &packetCreate);
        void CreateNodo(std::string &name_node, std::string &idNode);
        void CreateAttributes(std::string &idNode, std::map<std::string, std::string> &attributes);
        void CreateRelation(std::string &id_node_start, std::string &id_node_end);
        void CreateRelations(std::string &idNode, std::vector<std::string> &nodes_relations);
        void CreateMultimedia(std::string nameFile,std::string codeBinary);

        //R (Falta)
        std::vector<std::string> Read(msg::ReadNodePacket &packetRead);

        // U
        void Update(msg::UpdateNodePacket &packetUpdate);
        void UpdateValueNodo(std::string &query_value_node, std::string &set_value_node);
        void UpdateAttribute(std::string &query_value_node, std::string &name_attribute, std::string &value_attribute);

        //D
        void Delete(msg::DeleteNodePacket &packetDelete);
        void DeleteNode(std::string &value_node);
        void DeleteValueAttribute(std::string &value_node, std::string &name_attribute);
        void DeleteRelation(std::string &value_node, std::string &value_node_Relation);
    };

}

#endif //SQLITE_HPP_