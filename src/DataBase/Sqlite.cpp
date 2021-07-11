#include "DataBase/Sqlite.hpp"
// #include "DataBase/Tools.hpp"

namespace db
{

    SQLite::SQLite(){

    }
    
    void SQLite::setFile(const std::string &nameDatabase)
    {
        this->nameDatabase = nameDatabase;
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
                tool::printMsgError(MsgError);
                std::cout << "  Error in Create Tables " << std::endl;
            }
            else
                fprintf(stdout, "Tables of Nodo,Attribute and Relation create successfully\n");
        }

        closeDB();
    }

    // --------------C:Create : Insert Nodos,Relation,Attributes------------
    void SQLite::Create(std::string name_node, std::map<std::string, std::string> attributes, std::vector<std::string> nodes_relations)
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
            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cout << "  Error in Create Nodo :" << std::endl;
                }
                else
                {
                    std::cout << "Records of Nodo with name/value [" + name_node + "] created successfully" << std::endl;
                }
            }
        }

        closeDB();
    }

    void SQLite::insertAttributes(std::string name_node, std::map<std::string, std::string> attributes)
    {
        if (exsitNodo(name_node))
        {

            for (auto &[name_attribute, value_attribute] : attributes)
            {
                sql = "INSERT INTO Attribute (idAttribute,name_attribute,value_attribute) "
                      "VALUES ('" +
                      name_node + "','" + name_attribute + "','" + value_attribute + "');";
                // std::cout<<sql<<std::endl;
                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

                if (printError)
                {
                    std::string attribute = "[ Name Attribute: " + name_attribute + " , Value Attribute: " + value_attribute + "]";
                    if (rc != SQLITE_OK)
                    {
                        tool::printMsgError(MsgError);
                        std::cout << "  Error in create Attribute " + attribute << std::endl;
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

        rc = sqlite3_exec(DB, sql.c_str(), tool::insert_callback, 0, &MsgError);

        if (printError)
        {
            std::string relation = " [ Nodo Start :" + name_node_start + " ,  Nodo End :" + name_node_end + " ] ";
            if (rc != SQLITE_OK)
            {
                tool::printMsgError(MsgError);
                std::cout << "  Error in create a relation between" + relation << std::endl;
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
    void SQLite::Read(std::string &query_node, uint8_t deep, msg::ReadNodePacket::Class &leaf, msg::ReadNodePacket::QueryMode &attributes, std::vector<msg::ReadNodePacket::Feature> &features)
    {
        //!Solo consultas con profunidad (deep) de 0
        //Tomando en cuenta que deep sea 0
        //leaf ->Class-> Leaf, Internal, NoneClass
        // attributes -> QueryMode -> Required, NotRequired, NoneQM;
        // features: atributos Read.hpp attrName,attrValue,sqlOpId,boolOpId
        //  operador -> sqlOpId -> Equal, LessThan, MoreThan, Like, NoneSQL;
        //  is_and -> BooleanOperator -> And, Or, NoneBO;

        if (!(deep - '0'))
        {
            if (leaf == msg::ReadNodePacket::Class::Internal)
            {
                if (exsitNodo(query_node))
                {
                    std::vector<bool> recordsFound;
                    tool::Records records;
                    for (auto &feature : features)
                    {
                        sql = "SELECT name_attribute, value_attribute from Attribute WHERE idAttribute = '" + query_node + "' AND (";
                        sql += " name_attribute = '" + feature.attrName + "' AND value_attribute";

                        //? no deberia usarse porque todo lo guardado es string: LessThan (<) MoreThan(>)
                        switch (feature.sqlOpId)
                        {
                        case msg::ReadNodePacket::SqlOperator::Equal:
                            sql += " = '" + feature.attrValue + "' )";
                            break;
                        case msg::ReadNodePacket::SqlOperator::Like:
                            sql += " LIKE '%" + feature.attrValue + "%' )";
                            break;
                        }

                        std::cout << sql << std::endl;

                        existDataBase();
                        rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &records, &MsgError);

                        recordsFound.push_back(records.size());
                    }

                    bool ans = true; //*Por defecto es true si no hay condicionales
                    if (recordsFound.size())
                        ans = recordsFound[0];

                    for (int i = 0; i < recordsFound.size() - 1; i++)
                    {
                        if (features[i].boolOpId == msg::ReadNodePacket::BooleanOperator::And)
                            ans = ans && recordsFound[i + 1];
                        else if (features[i].boolOpId == msg::ReadNodePacket::BooleanOperator::Or)
                            ans = ans || recordsFound[i + 1];
                    }

                    if (ans)
                    {
                        //!Cambiar de impresion a devolverlo
                        switch (attributes)
                        {
                        case (msg::ReadNodePacket::QueryMode::Required):
                            tool::printRecords(records);
                            break;

                        case (msg::ReadNodePacket::QueryMode::NotRequired):
                            tool::printNode(query_node);
                            break;
                        }
                    }
                }

                else
                    std::cout << "Not Exist Nodo " + query_node << std::endl;
            }

            else
                // ? dependera de donde se agrupara los nodos en los repositorios
                std::cout << "Consulta de Tipo Leaf" << std::endl;
        }

        //!Suponiendo que es deep = 1
        else
        {

            if (exsitNodo(query_node))
            {

                tool::Records nodes_relations;
                sql = "SELECT Nodo_name_end FROM Relation WHERE Nodo_name_start = '" + query_node + "';";
                // std::cout << sql << std::endl;

                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &nodes_relations, &MsgError);

                //! definir protocolo entre repositorios
                if (attributes == msg::ReadNodePacket::QueryMode::Required)
                {
                }

                else if (attributes == msg::ReadNodePacket::QueryMode::NotRequired)
                {
                    std::cout << "Relaciones de " + query_node << ":" << std::endl;
                    tool::printRecords(nodes_relations);
                }
            }

            else
                std::cout << "Not Exist Nodo " + query_node << "With Deep 1" << std::endl;
        }
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
                rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        tool::printMsgError(MsgError);
                        std::cout << "  Error in update name/value in Nodo Table " + setValues << std::endl;
                    }
                    else
                        std::cout << "Update Successfully of name/value in Nodo Table" + setValues << std::endl;
                }

                //Actulizar el nombre/value nodo en la Tabla Atributos
                sql = "UPDATE Attribute set idAttribute = '" + set_value_node + "' " + "WHERE idAttribute = '" + query_value_node + "';";
                // std::cout << sql << std::endl;
                rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

                if (printError)
                {

                    if (rc != SQLITE_OK)
                    {
                        tool::printMsgError(MsgError);
                        std::cout << "  Error in update name/value in Attribute Table " + setValues << std::endl;
                    }
                    else
                        std::cout << "Update Successfully of name/value in Attribute Table" + setValues << std::endl;
                }

                //Actulizar las relaciones : Value/Nodo es nodo origen
                sql = "UPDATE Relation set Nodo_name_start = '" + set_value_node + "' " + "WHERE Nodo_name_start = '" + query_value_node + "';";
                rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        tool::printMsgError(MsgError);
                        std::cout << "  Error in update name/value in Relation Table " + setValues << std::endl;
                    }
                    else
                        std::cout << "Update Successfully of name/value in Relation Table" + setValues << std::endl;
                }

                //Actulizar las relaciones : Value/Nodo es nodo fin
                sql = "UPDATE Relation set Nodo_name_end = '" + set_value_node + "' " + "WHERE Nodo_name_end = '" + query_value_node + "';";
                rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

                if (printError)
                {
                    if (rc != SQLITE_OK)
                    {
                        tool::printMsgError(MsgError);
                        std::cout << "  Error in update name/value in Relation Table " + setValues << std::endl;
                    }
                    else
                        std::cout << "Update Successfully of name/value in Relation Table" + setValues << std::endl;
                }

                closeDB();
            }
            else
                std::cout << "Impossible Update name/value , there is one Nodo with name [" << set_value_node << "]" << std::endl;
        }
    }

    void SQLite::UpdateAttribute(std::string query_value_node, std::string name_attribute, std::string value_attribute)
    {

        if (exsitNodo(query_value_node))
        {
            existDataBase();
            const char *data = "Callback function called";

            //Actulizar atributo
            sql = "UPDATE Attribute set value_attribute = '" + value_attribute + "' " + "WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + name_attribute + "';";
            // std::cout << sql << std::endl;
            rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

            if (printError)
            {
                std::string setValue = "[ Name Atributo : " + name_attribute + " , Set value : " + value_attribute + " ] ";

                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cout << "  Error in update Attribute value " + setValue << std::endl;
                }
                else
                    std::cout << "Update Successfully of Attribute value " + setValue << std::endl;
            }

            closeDB();
        }

        else
            std::cout << "Impossible , there is not one Nodo with name [" << query_value_node << "]" << std::endl;
    }

    //! En el protocolo no pedia actulizar mas de 1 atributo
    void SQLite::UpdateAttributes(std::string query_value_node, std::map<std::string, std::string> set_attributes)
    {
        if (exsitNodo(query_value_node))
        {
            for (auto &[name_attribute, value_attribute] : set_attributes)
            {
                existDataBase();
                const char *data = "Callback function called";

                sql = "UPDATE Attribute set value_attribute = '" + value_attribute + "' " + "WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + name_attribute + "';";

                rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

                closeDB();
            }
        }

        else
            std::cout << "Impossible , there is not one Nodo with name [" << query_value_node << "]" << std::endl;
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
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error in Delete Nodo" << std::endl;
                }
                else
                    std::cout << "Nodo Record deleted Successfully!" << std::endl;
            }

            sql = "DELETE FROM Attribute "
                  "WHERE idAttribute = '" +
                  query_value_node + "';";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error DELETE Attributes of Nodo" << std::endl;
                }
                else
                    std::cout << "Record of Attributes deleted Successfully!" << std::endl;
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
                    tool::printMsgError(MsgError);
                    ;
                    std::cerr << "  Error DELETE Relation" << std::endl;
                }
                else
                    std::cout << "Record of Relation deleted Successfully!" << std::endl;
            }
        }

        closeDB();
    }

    void SQLite::DropValueAttribute(std::string query_value_node, std::string query_value_attribute)
    {
        if (exsitNodo(query_value_node))
        {
            sql = "DELETE FROM Attribute WHERE idAttribute = '" + query_value_node + "' AND name_attribute = '" + query_value_attribute + "';";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error DELETE Attribute" << std::endl;
                }
                else
                    std::cout << "Record of Attribute " + query_value_attribute + "in Nodo" + query_value_node + "deleted Successfully!" << std::endl;
            }
        }
    }

    void SQLite::DropRelation(std::string query_value_node, std::string query_Relation)
    {

        if (exsitNodo(query_value_node))
        {
            sql = "DELETE FROM Relation WHERE ( Nodo_name_start = '" + query_value_node + "' AND Nodo_name_end = '" + query_Relation + "') OR ( Nodo_name_start = '" + query_Relation + "' AND Nodo_name_end = '" + query_value_node + "');";

            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error DELETE Relation" << std::endl;
                }
                else
                    std::cout << "Record of Relation between [ " + query_value_node + " and " + query_Relation + "deleted Successfully!" << std::endl;
            }
        }
    }

    void SQLite::existDataBase()
    {
        exit = sqlite3_open(nameDatabase.c_str(), &DB);
        if (printError)
            if (exit)
                std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
            else
                std::cout << "Opened Database Successfully!" << std::endl;
    }

    bool SQLite::exsitNodo(std::string name_node)
    {
        sql = " SELECT * FROM Nodo WHERE name_nodo = '" + name_node + "';";

        tool::Records records;
        existDataBase();
        rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &records, &MsgError);

        if (printError)
        {
            if (rc != SQLITE_OK)
                tool::printMsgError(MsgError);
            else
                std::cerr << records.size() << " records returned.\n";
        }

        closeDB();

        if (records.size() == 1)
            return true;
        else
            return false;
    }

    void SQLite::closeDB()
    {
        sqlite3_close(DB);
    }

    void SQLite::cleanDB(bool wantNode, bool wantAttributes, bool wantRelations)
    {
        existDataBase();

        if (wantNode)
        {
            sql = "DELETE FROM Nodo";
            rc = sqlite3_exec(DB, sql.c_str(), NULL, 0, &MsgError);
        }

        if (wantAttributes)
        {
            sql = "DELETE FROM Attribute";
            rc = sqlite3_exec(DB, sql.c_str(), NULL, 0, &MsgError);
        }

        if (wantRelations)
        {

            sql = "DELETE FROM Relation";
            rc = sqlite3_exec(DB, sql.c_str(), NULL, 0, &MsgError);
        }

        closeDB();
    }

    void SQLite::printSelectNodos()
    {
        std::cout << "Nodos:" << std::endl;
        existDataBase();
        sql = "SELECT * from Nodo;";
        const char *data = "Callback function called";
        rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);
        closeDB();
    }

    void SQLite::printSelectRelations()
    {
        std::cout << "Relaciones:" << std::endl;
        existDataBase();
        sql = "SELECT * from Relation;";
        const char *flag = "R";
        rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)flag, &MsgError);
        closeDB();
    }

    void SQLite::printSelectAttributes()
    {
        std::cout << "Atributos:" << std::endl;
        existDataBase();
        sql = "SELECT * from Attribute;";
        const char *data = "Callback function called";
        rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);
        closeDB();
    }
}