#ifndef Sqlite_HPP_
#define Sqlite_HPP_

#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>
#include <set>

namespace database
{
    void decoOperator(std::string opt)
    {
        if (opt == "1")
            opt = "=";
        else if (opt == "2")
            opt = ">";
        else if (opt == "3")
            opt = "<";
        else if (opt == "4")
            opt = "LIKE";
    }

    static int callbackInsert(void *NotUsed, int sizeRow, char **fieldRow, char **namesColumn)
    {
        for (int i = 0; i < sizeRow; i++)
            printf("%s = %s\n", namesColumn[i], fieldRow[i] ? fieldRow[i] : "NULL");
        printf("\n");
        return 0;
    }

    //
    using Record = std::vector<std::string>;
    using Records = std::vector<Record>;

    int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
    {
        Records *records = static_cast<Records *>(p_data);
        try
        {
            records->emplace_back(p_fields, p_fields + num_fields);
        }
        catch (...)
        {
            // abort select on failure, don't let exception propogate thru sqlite3 call-stack
            return 1;
        }
        return 0;
    }

    int insert_callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        int i;
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }

    // int print_select_callback(std::set<std::string> nodos, int argc, char **argv, char **azColName)
    int print_select_callback(void *flag, int argc, char **argv, char **azColName)
    {

        for (int i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }

        printf("\n");
        return 0;
    }

    int get_value_nodos(void *nodos, int num_fields, char **p_fields, char **p_col_names)
    {
        std::set<std::string> *records = static_cast<std::set<std::string> *>(nodos);

        for (int i = 0; i < num_fields; i++)
            records->insert(p_fields[i]);

        return 0;
    }

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

        //Para 2 Tablas
        std::set<std::string> nodos;

    public:
        bool printError = false;

        SQLite(const std::string &nameDatabase = "GraphNetwork.db");

        //CRUD
        //C
        void Create(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes, std::vector<std::string> nodes_relations);
        //R (Falta)
        std::vector<std::string> Read(std::vector<std::string> path, std::string query_node, std::string deep, std::string leaf, std::string attributes, std::vector<std::tuple<std::string, std::string, std::string, std::string>> condition);

        // U
        void UpdateValueNodo(std::string query_value_node, std::string set_value_node);
        void UpdateAttribute(std::string query_value_node, std::pair<std::string, std::string> set_attribute);
        void UpdateAttributes(std::string query_value_node, std::vector<std::pair<std::string, std::string>> set_attributes);
        //D
        void DropNode(std::string query_value_node);
        void DropValueAttributeOrRelation(std::string query_value_node, std::string query_value_attribute_o_R_size, std::string node_or_attribute);

        void createTables();
        void insertNodo(std::string name_node);
        void insertAttributes(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes);
        //No  comprueba que los 2 existen sino que crea los 2 nodos
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

    SQLite::SQLite(const std::string &nameDatabase)
    {
        this->nameDatabase = nameDatabase;
        // existDataBase();
        // closeDB();
    }

    void SQLite::createTables()
    {
        existDataBase();

        sql = "CREATE TABLE Nodo ("
              " name_nodo CHAR(255) NOT NULL);"
              "CREATE TABLE Attribute ("
              "idAttribute CHAR(255) NOT NULL,"
              " name_attribute CHAR(255) NOT NULL,"
              "value_attribute CHAR(255) NOT NULL);"
              "CREATE TABLE Relation ("
              "Nodo_name_start CHAR(255) NOT NULL,"
              "Nodo_name_end CHAR(255) NOT NULL);";

        // rc = sqlite3_exec(DB, sql.c_str(), callback, 0, &MsgError);
        rc = sqlite3_exec(DB, sql.c_str(), NULL, 0, &MsgError);

        if (printError)
        {
            if (rc != SQLITE_OK)
            {
                std::cout << "SQL error: " << MsgError << std::endl;
                std::cout << "  Error in Create Tables " << std::endl;
                sqlite3_free(MsgError);
            }
            else
                fprintf(stdout, "Tables of Nodo,Attribute and Relation create successfully\n");
        }

        closeDB();
    }

    // --------------C:Create : Insert Nodos,Relation,Attributes------------
    void SQLite::Create(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes, std::vector<std::string> nodes_relations)
    {
        if (!exsitNodo(name_node))
        {
            insertNodo(name_node);
            if (attributes.size() > 0)
                insertAttributes(name_node, attributes);

            if (nodes_relations.size() > 0)
                createRelations(name_node, nodes_relations);
        }

        else
            std::cout << "Imposible Create, there is Nodo with value/name of " + name_node << std::endl;
    }

    void SQLite::insertNodo(std::string name_node)
    {

        if (!exsitNodo(name_node))
        {
            sql = "INSERT INTO Nodo (name_nodo) "
                  "VALUES ('" +
                  name_node + "')";

            existDataBase();
            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    std::cout << "SQL error :" << MsgError << std::endl;
                    std::cout << "  Error in Create Nodo :" << std::endl;
                    sqlite3_free(MsgError);
                }
                else
                {
                    std::cout << "Records of Nodo with name/value [" + name_node + "] created successfully" << std::endl;
                }
            }
        }

        closeDB();
    }

    void SQLite::insertAttributes(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes)
    {
        if (exsitNodo(name_node))
        {

            for (int i = 0; i < attributes.size(); i++)
            {
                sql = "INSERT INTO Attribute (idAttribute,name_attribute,value_attribute) "
                      "VALUES ('" +
                      name_node + "','" + attributes[i].first + "','" + attributes[i].second + "');";
                // std::cout<<sql<<std::endl;
                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);

                if (printError)
                {
                    std::string attribute = "[ Name Attribute: " + attributes[i].first + " , Value Attribute: " + attributes[i].second + "]";
                    if (rc != SQLITE_OK)
                    {
                        std::cout << "SQL error : " << MsgError << std::endl;
                        std::cout << "  Error in create Attribute " + attribute << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Records Attribute " + attribute + " created successfully" << std::endl;
                    }
                }
            }
        }

        else if (printError)
        {
            std::cout << "For Attributes Insertion none Node exist with name of [ " + name_node + " ]" << std::endl;
        }

        closeDB();
    }

    void SQLite::createRelation(std::string name_node_start, std::string name_node_end)
    {
        sql = "INSERT INTO Relation (Nodo_name_start,Nodo_name_end) "
              "VALUES ('" +
              name_node_start + "','" + name_node_end + "');";

        existDataBase();

        rc = sqlite3_exec(DB, sql.c_str(), insert_callback, 0, &MsgError);

        if (printError)
        {
            std::string relation = " [ Nodo Start :" + name_node_start + " ,  Nodo End :" + name_node_end + " ] ";
            if (rc != SQLITE_OK)
            {
                std::cout << "SQL error : " << MsgError << std::endl;
                std::cout << "  Error in create a relation between" + relation << std::endl;
                sqlite3_free(MsgError);
            }
            else
            {
                std::cout << "Relation Record between" + relation + "created successfully" << std::endl;
            }
        }

        closeDB();
    }

    void SQLite::createRelations(std::string name_node_start, std::vector<std::string> nodes_relations)
    {
        std::string name_node_end;
        for (int i = 0; i < nodes_relations.size(); i++)
        {
            name_node_end = nodes_relations[i];
            insertNodo(name_node_end);
            createRelation(name_node_start, name_node_end);
        }
    }
    //----------------R:Read-------------------------

    std::vector<std::string> SQLite::Read(std::vector<std::string> path, std::string query_node, std::string deep, std::string leaf, std::string attributes, std::vector<std::tuple<std::string, std::string, std::string, std::string>> conditions)
    {

        // if (deep == 0)
        //     return path;
        // else
        // {
        //     sql = "SELECT Nodo_name_end FROM Relation WHERE Nodo_name = '" + query_node + "' ";

        //     if (conditions.empty())
        //     {
        //         sql += ";"
        //     }

        //     else
        //     {
        //         sql += "AND (";

        //         for (std::vector<std::tuple<std::string, std::string, std::string, std::string>>::iterator condition = conditions.begin(); condition != conditions.end(); ++condition)
        //             std::string query_name_attribute = std::get<0>(conditions);
        //         std::string operador = std::get<1>(conditions);
        //         std::string query_value_attribute = std::get<2>(conditions);
        //         std::string is_and = std::get<3>(conditions);

        //         sql += query_name_attribute + operador + "'";

        //         sql += query_value_attribute + "' ";
        //     }
        // }

        //Borrar
        return path;
    }

    // ----------------U:Updates-------------------------
    void SQLite::UpdateValueNodo(std::string query_value_node, std::string set_value_node)
    {
        if (exsitNodo(query_value_node))
        {
            if (!exsitNodo(set_value_node))
            {
                existDataBase();
                const char *data = "Callback function called";
                std::string setValues = " [ Old value :" + query_value_node + " ,  New value :" + set_value_node + " ] ";

                //Actulizar el value/name del nodo
                sql = "UPDATE Nodo set name_nodo = '" + set_value_node + "' " + "WHERE name_nodo = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        std::cout << "SQL error : " << MsgError << std::endl;
                        std::cout << "  Error in update name/value in Nodo Table " + setValues << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Update Successfully of name/value in Nodo Table" + setValues << std::endl;
                    }
                }

                //Actulizar el nombre/value nodo en la Tabla Atributos
                sql = "UPDATE Attribute set idAttribute = '" + set_value_node + "' " + "WHERE idAttribute = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                if (printError)
                {

                    if (rc != SQLITE_OK)
                    {
                        std::cout << "SQL error : " << MsgError << std::endl;
                        std::cout << "  Error in update name/value in Attribute Table " + setValues << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Update Successfully of name/value in Attribute Table" + setValues << std::endl;
                    }
                }

                //Actulizar las relaciones : Primero las que el nodo esta como inicio
                sql = "UPDATE Relation set Nodo_name_start = '" + set_value_node + "' " + "WHERE Nodo_name_start = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        std::cout << "SQL error : " << MsgError << std::endl;
                        std::cout << "  Error in update name/value in Relation Table " + setValues << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Update Successfully of name/value in Relation Table" + setValues << std::endl;
                    }
                }

                //Nodo es el final
                sql = "UPDATE Relation set Nodo_name_end = '" + set_value_node + "' " + "WHERE Nodo_name_end = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        std::cout << "SQL error : " << MsgError << std::endl;
                        std::cout << "  Error in update name/value in Relation Table " + setValues << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Update Successfully of name/value in Relation Table" + setValues << std::endl;
                    }
                }

                closeDB();
            }
            else
            {
                std::cout
                    << "Impossible Update name/value , there is one Nodo with name [" << set_value_node << "]" << std::endl;
            }
        }
    }

    void SQLite::UpdateAttribute(std::string query_value_node, std::pair<std::string, std::string> set_attribute)
    {

        if (exsitNodo(query_value_node))
        {
            existDataBase();
            const char *data = "Callback function called";

            //Actulizar atributo
            sql = "UPDATE Attribute set value_attribute = '" + set_attribute.second + "' " + "WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + set_attribute.first + "';";
            // std::cout << sql << std::endl;
            rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

            if (printError)
            {
                std::string setValue = "[ Name Atributo : " + set_attribute.first + " , Set value : " + set_attribute.second + " ] ";

                if (rc != SQLITE_OK)
                {
                    std::cout << "SQL error : " << MsgError << std::endl;
                    std::cout << "  Error in update Attribute value " + setValue << std::endl;
                    sqlite3_free(MsgError);
                }
                else
                {
                    std::cout << "Update Successfully of Attribute value " + setValue << std::endl;
                }
            }

            closeDB();
        }

        else
        {
            std::cout
                << "Impossible , there is not one Nodo with name [" << query_value_node << "]" << std::endl;
        }
    }

    void SQLite::UpdateAttributes(std::string query_value_node, std::vector<std::pair<std::string, std::string>> set_attributes)
    {
        if (exsitNodo(query_value_node))
        {
            for (int i = 0; i < set_attributes.size(); i++)
            {
                existDataBase();
                const char *data = "Callback function called";

                //Actulizar atributos
                sql = "UPDATE Attribute set value_attribute = '" + set_attributes[i].second + "' " + "WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + set_attributes[i].first + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                closeDB();
            }
        }

        else
        {
            std::cout
                << "Impossible , there is not one Nodo with name [" << query_value_node << "]" << std::endl;
        }
    }

    //-----------------D:Drop Node o Attributes of Node or Relation
    void SQLite::DropNode(std::string query_value_node)
    {

        if (exsitNodo(query_value_node))
        {
            existDataBase();

            sql = "DELETE FROM Nodo WHERE name_nodo = '" + query_value_node + "';";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    std::cerr << "SQL Error :" << MsgError << std::endl;
                    std::cerr << "  Error in DELETE Nodo" << std::endl;
                    sqlite3_free(MsgError);
                }
                else
                {
                    std::cout << "Nodo Record deleted Successfully!" << std::endl;
                }
            }

            sql = "DELETE FROM Attribute "
                  "WHERE idAttribute = '" +
                  query_value_node + "';";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    std::cerr << "SQL Error :" << MsgError << std::endl;
                    std::cerr << "  Error DELETE Attributes of Nodo" << std::endl;
                    sqlite3_free(MsgError);
                }
                else
                {
                    std::cout << "Record of Attributes deleted Successfully!" << std::endl;
                }
            }

            sql = "DELETE FROM Relation "
                  "WHERE Nodo_name_start = '" +
                  query_value_node + "' OR Nodo_name_end = '" +
                  query_value_node + "';";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    std::cerr << "SQL Error :" << MsgError << std::endl;
                    std::cerr << "  Error DELETE Relation" << std::endl;
                    sqlite3_free(MsgError);
                }
                else
                {
                    std::cout << "Record of Relation deleted Successfully!" << std::endl;
                }
            }
        }

        closeDB();
    }

    void SQLite::DropValueAttributeOrRelation(std::string query_value_node, std::string query_value_attribute_o_R_size, std::string node_or_attribute)
    {
        if (exsitNodo(query_value_node))
        {
            existDataBase();

            // Attribute
            if (node_or_attribute == "2")
            {
                sql = "DELETE FROM Attribute WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + query_value_attribute_o_R_size + "';";

                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        std::cerr << "SQL Error :" << MsgError << std::endl;
                        std::cerr << "  Error DELETE Attribute" << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Record of Attribute " + query_value_attribute_o_R_size + "in Nodo" + query_value_node + "deleted Successfully!" << std::endl;
                    }
                }
            }

            //Relation
            else if (node_or_attribute == "3")
            {
                sql = "DELETE FROM Relation WHERE ( Nodo_name_start = '" + query_value_node + "' AND Nodo_name_end = '" + query_value_attribute_o_R_size + "') OR ( Nodo_name_start = '" + query_value_attribute_o_R_size + "' AND Nodo_name_end = '" + query_value_node + "');";

                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        std::cerr << "SQL Error :" << MsgError << std::endl;
                        std::cerr << "  Error DELETE Relation" << std::endl;
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        std::cout << "Record of Relation between [ " + query_value_node + " and " + query_value_attribute_o_R_size + "deleted Successfully!" << std::endl;
                    }
                }
            }
        }

        closeDB();
    }

    //Functiones Complementarias
    void SQLite::existDataBase()
    {
        exit = sqlite3_open(nameDatabase.c_str(), &DB);
        if (printError)
        {
            if (exit)
                std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
            else
                std::cout << "Opened Database Successfully!" << std::endl;
        }
    }

    bool SQLite::exsitNodo(std::string name_node)
    {
        //-------------------------Para 3 tablas-------------------------
        sql = " SELECT * FROM Nodo WHERE name_nodo = '" + name_node + "';";
        std::cout << sql << std::endl;

        Records records;
        existDataBase();
        rc = sqlite3_exec(DB, sql.c_str(), select_callback, &records, &MsgError);

        if (printError)
        {
            if (rc != SQLITE_OK)
            {
                std::cerr << "SQL Error :" << MsgError << std::endl;
                // std::cerr << "Error in select Nodo with name [" << name_node << "] with statement " << std::endl;
                sqlite3_free(MsgError);
            }
            else
                std::cerr << records.size() << " records returned.\n";
        }
        closeDB();

        // std::cout << "Registros Hallados = " << records.size() << std::endl;

        if (records.size() == 1)
            return true;
        else
            return false;

        //-------------------------Para 2 Tablas-------------------------
        //const bool is_in = nodos.find(name_node) != nodos.end();
        //return is_in;
    }

    void SQLite::closeDB()
    {
        sqlite3_close(DB);
    }

    void SQLite::printSelectNodos()
    {
        //-------------------------Para solo 2 tablas-------------------------
        //Forma con un flag,guardar en variable global
        // std::cout << "Nodos:" << std::endl;
        // existDataBase();
        // sql = "SELECT * from Relation;";
        // const char *flag = "N";
        // rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)flag, &MsgError);
        // for (auto it = nodos.begin(); it != nodos.end(); ++it)
        //     std::cout << "Nodo: " << *it << std::endl;

        //Guardar los nodos en un atributo de la clase
        // std::cout << "Nodos:" << std::endl;
        // existDataBase();
        // sql = "SELECT * from Relation;";
        // rc = sqlite3_exec(DB, sql.c_str(), get_value_nodos, &nodos2, &MsgError);

        // for (auto it = nodos2.begin(); it != nodos2.end(); ++it)
        //     std::cout << " Nodo : " << *it << std::endl;

        //Metodo Final
        // for (auto it = nodos.begin(); it != nodos.end(); ++it)
        //     std::cout << " Nodo : " << *it << std::endl;

        // closeDB();

        //---------------------------Paara 3 tablas-------------------------
        std::cout << "Nodos:" << std::endl;
        existDataBase();
        sql = "SELECT * from Nodo;";
        const char *data = "Callback function called";
        rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);
        closeDB();
    }

    void SQLite::printSelectRelations()
    {
        std::cout << "Relaciones:" << std::endl;
        existDataBase();
        sql = "SELECT * from Relation;";
        const char *flag = "R";
        //Por dentro llamamos print_select_callback mas de 1 vez esta funcion por cada registro que tenemos se llamara a la funcion
        // rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);
        // rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)5, &MsgError);

        rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)flag, &MsgError);

        closeDB();
    }

    void SQLite::printSelectAttributes()
    {
        std::cout << "Atributos:" << std::endl;
        existDataBase();
        sql = "SELECT * from Attribute;";
        const char *data = "Callback function called";
        rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);
        closeDB();
    }

}

#endif //Sqlite_HPP_