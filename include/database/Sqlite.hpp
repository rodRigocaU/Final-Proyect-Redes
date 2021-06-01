#ifndef Sqlite_HPP_
#define Sqlite_HPP_

#include <iostream>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

namespace database
{
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

    int print_select_callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        for (int i = 0; i < argc; i++)
        {
            // Impresion para el select
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }

    //Siempre el nodo origen es el menor
    void compareString(std::string &node_start, std::string &node_end)
    {
        int status = node_start.compare(node_end);
        //Cuando el node_start es mayor en cuanto caracteres que el node_end
        if (status > 0)
            node_start.swap(node_end);
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
        bool printError = false;
        // bool printError = true;
        bool printRecord = true;

        std::string sql;

    public:
        SQLite(const std::string &nameDatabase = "GraphNetwork.db");

        //CRUD
        //C
        void Create(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes, std::vector<std::string> nodes_relations);
        // U
        void UpdateValueNodo(std::string query_value_node, std::string set_value_node);
        void UpdateAttributes(std::string query_value_node, std::vector<std::pair<std::string, std::string>> set_attributes);

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
        existDataBase();
        closeDB();
        // std::cout<<"Gaa"<<std::endl;
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
                fprintf(stderr, "SQL error: %s\n", MsgError);
                sqlite3_free(MsgError);
            }
            else
                fprintf(stdout, "Table created successfully\n");
        }

        closeDB();
    }

    // --------------C:Create : Insert Nodos,Relation,Attributes------------
    void SQLite::Create(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes, std::vector<std::string> nodes_relations)
    {
        insertNodo(name_node);
        if (attributes.size() > 0)
            insertAttributes(name_node, attributes);

        if (nodes_relations.size() > 0)
            createRelations(name_node, nodes_relations);
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
                    fprintf(stderr, "SQL error: %s\n", MsgError);
                    sqlite3_free(MsgError);
                }
                else
                {
                    fprintf(stdout, "Records created successfully\n");
                }
            }
        }

        closeDB();
    }

    void SQLite::insertAttributes(std::string name_node, std::vector<std::pair<std::string, std::string>> attributes)
    {

        std::cout << "...Insertando" << std::endl;
        if (exsitNodo(name_node))
        {
            std::cout << "Insertando" << std::endl;
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
                    if (rc != SQLITE_OK)
                    {
                        fprintf(stderr, "SQL error: %s\n", MsgError);
                        sqlite3_free(MsgError);
                    }
                    else
                    {
                        fprintf(stdout, "Records Attribute created successfully\n");
                    }
                }
            }
        }

        else if (printError)
        {
            std::cout << "None Node exist with name of " + name_node << std::endl;
        }

        closeDB();
    }

    void SQLite::createRelation(std::string name_node_start, std::string name_node_end)
    {
        sql = "INSERT INTO Relation (Nodo_name_start,Nodo_name_end) "
              "VALUES ('" +
              name_node_start + "','" + name_node_end + "');";
        // std::cout << sql << std::endl;

        existDataBase();
        rc = sqlite3_exec(DB, sql.c_str(), insert_callback, 0, &MsgError);
        // rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, NULL);

        if (printError)
        {
            if (rc != SQLITE_OK)
            {
                fprintf(stderr, "SQL error: %s\n", MsgError);
                sqlite3_free(MsgError);
            }
            else
            {
                fprintf(stdout, "Records created successfully\n");
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
            // Ordenar alfabeticamente los nodos poruqe como es no dirigido
            //A,B -> A,B
            //B,A -> A,B
            compareString(name_node_start, name_node_end);
            createRelation(name_node_start, name_node_end);
        }
    }

    // ----------------U:Updates-------------------------
    void SQLite::UpdateValueNodo(std::string query_value_node, std::string set_value_node)
    {
        if (exsitNodo(query_value_node))
        {
            // std::cout << "Existe " << query_value_node << std::endl;
            if (!exsitNodo(set_value_node))
            {
                existDataBase();
                const char *data = "Callback function called";

                //Actulizar el vvalue o name del nodo
                sql = "UPDATE Nodo set name_nodo = '" + set_value_node + "' " + "WHERE name_nodo = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                //Actulizar a quien pertenece los atributos del nodo
                sql = "UPDATE Attribute set idAttribute = '" + set_value_node + "' " + "WHERE idAttribute = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                //Actulizar las relaciones : Primero las que el nodo esta como inicio
                sql = "UPDATE Relation set Nodo_name_start = '" + set_value_node + "' " + "WHERE Nodo_name_start = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                //Nodo es el final
                sql = "UPDATE Relation set Nodo_name_end = '" + set_value_node + "' " + "WHERE Nodo_name_end = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);

                closeDB();
            }
            else
            {
                std::cout
                    << "Impossible , there is one Nodo with name [" << set_value_node << "]" << std::endl;
            }
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
                sql = "UPDATE Attribute set value_attribute = '" + set_attributes[i].second + "' " + "WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '"+set_attributes[i].first+"';";
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
        sql = " SELECT * FROM Nodo WHERE name_nodo = '" + name_node + "';";
        // std::cout << sql << std::endl;

        Records records;
        existDataBase();
        rc = sqlite3_exec(DB, sql.c_str(), select_callback, &records, &MsgError);

        if (printError)
        {
            if (rc != SQLITE_OK)
                std::cerr << "Error in select Nodo with name [" << name_node << "] with statement " << sql << "[" << MsgError << "]\n";
            else
                std::cerr << records.size() << " records returned.\n";
        }
        closeDB();

        // std::cout << "Registros Hallados = " << records.size() << std::endl;

        if (records.size() == 1)
            return true;
        else
            return false;
    }

    void SQLite::closeDB()
    {
        sqlite3_close(DB);
    }

    void SQLite::printSelectNodos()
    {
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
        const char *data = "Callback function called";
        rc = sqlite3_exec(DB, sql.c_str(), print_select_callback, (void *)data, &MsgError);
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