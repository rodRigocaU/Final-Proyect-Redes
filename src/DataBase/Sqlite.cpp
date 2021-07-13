#include "DataBase/Sqlite.hpp"
#include "iostream"
#include "fstream"
#include <filesystem>

using namespace std;
namespace db
{

    void SQLite::setFile(const std::string &file)
    {
        nameDatabase = file;
    }

    void SQLite::createTables()
    {
        existDataBase();
        // sql = "PRAGMA foreign_keys = ON;";

        sql = "CREATE TABLE Nodo ( id_Nodo INTEGER PRIMARY KEY AUTOINCREMENT,name_nodo CHAR(255) NOT NULL);";

        sql += "CREATE TABLE Attribute (name_attribute CHAR(255) NOT NULL, value_attribute CHAR(255) NOT NULL, idAttribute INTEGER, FOREIGN KEY (idAttribute) REFERENCES Nodo (id_Nodo) ON UPDATE CASCADE ON DELETE CASCADE );";

        sql += " CREATE TABLE Relation ( Nodo_name_start INTEGER, Nodo_name_end INTEGER, FOREIGN KEY (Nodo_name_start) REFERENCES Nodo (id_Nodo)  ON UPDATE CASCADE ON DELETE CASCADE,FOREIGN KEY (Nodo_name_end) REFERENCES Nodo (id_Nodo) ON UPDATE CASCADE ON DELETE CASCADE );";

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

    void SQLite::existDataBase()
    {
        int exit;
        exit = sqlite3_open(nameDatabase.c_str(), &DB);
        if (printError)
        {
            if (exit)
                std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
            else
                std::cout << "Opened Database Successfully!" << std::endl;
        }
    }

    bool SQLite::existNodo(std::string name_node, std::string &id_Node)
    {
        sql = " SELECT id_Nodo FROM Nodo WHERE name_nodo = '" + name_node + "';";

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
        {
            id_Node = records[0][0];
            return true;
        }
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

    void SQLite::writeComandSQL()
    {
        existDataBase();

        while (true)
        {
            std::getline(std::cin, sql);
            if (sql == "quit")
                break;
            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (rc != SQLITE_OK)
                tool::printMsgError(MsgError);
        }
        closeDB();
    }

    void SQLite::ConstraintForeign()
    {
        sql = "PRAGMA foreign_keys = ON;";
        rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);
    }

    // --------------C:Create : Insert Nodos,Relation,Attributes------------

    void SQLite::Create(msg::CreateNodePacket &packetCreate)
    {
        std::string idNode;
        if (!existNodo(packetCreate.nodeId, idNode))
        {
            CreateNodo(packetCreate.nodeId, idNode);
            if (packetCreate.attributes.size() > 0)
                CreateAttributes(idNode, packetCreate.attributes);

            if (packetCreate.relations.size() > 0)
                CreateRelations(idNode, packetCreate.relations);
        }

        else if (printError)
            std::cout << "Imposible Create, there is Nodo with value/name of " + packetCreate.nodeId << std::endl;
    }

    void SQLite::CreateNodo(std::string &name_node, std::string &idNode)
    {
        std::string notUse;

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
                std::cout << "Records of Nodo with name/value [" + name_node + "] created successfully" << std::endl;
        }

        existNodo(name_node, idNode);
        closeDB();
    }

    void SQLite::CreateAttributes(std::string &idNode, std::map<std::string, std::string> &attributes)
    {
        //first : Name Atribute
        // second : Valor Atributo
        

        for (auto &item : attributes)
        {
            //Attribute as .png .jpg or .txt
            if( tool::isMultimedia(item.first) ){
                CreateMultimedia(item.first,item.second);
                sql = "INSERT INTO Attribute (idAttribute,name_attribute,value_attribute) "
                        "VALUES ( " + idNode + " , '" + item.first + "' , '" + item.first + "' );";
                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);
                closeDB();
            }

            //Attribute as string
            else {
                std::cout<<"Atributo Normal"<<std::endl;
                sql = "INSERT INTO Attribute (idAttribute,name_attribute,value_attribute) "
                        "VALUES ( " + idNode + " , '" + item.first + "' , '" + item.second + "' );";

                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);
                closeDB();
                if (printError)
                {
                        std::string attribute = "[ Name Attribute: " + item.first + " , Value Attribute: " + item.second + "]";
                        if (rc != SQLITE_OK)
                        {
                            tool::printMsgError(MsgError);
                            std::cout << "  Error in create Attribute " + attribute << std::endl;
                        }
                        else
                            std::cout << "Records Attribute " + attribute + " created successfully" << std::endl;
                }

            }
        }
    }

    void SQLite::CreateMultimedia(std::string nameFile,std::string codeBinary){
            std::ofstream image(nameFile, std::ios::out | std::ios::app);
            for(int i = 0; i < codeBinary.length(); i++)
                image.put(codeBinary[i]);
             
            std::cout << "BINARY-TO-IMG-COMPLETED " << '\n';
            image.clear();
            
    }

    
    void SQLite::CreateRelation(std::string &id_node_start, std::string &id_node_end)
    {
        sql = "INSERT INTO Relation (Nodo_name_start,Nodo_name_end) "
              "VALUES (" +
              id_node_start + "," + id_node_end + ");";

        existDataBase();

        rc = sqlite3_exec(DB, sql.c_str(), tool::insert_callback, 0, &MsgError);

        if (printError)
        {
            std::string relation = " [ Nodo Start :" + id_node_start + " ,  Nodo End :" + id_node_end + " ] ";
            if (rc != SQLITE_OK)
            {
                tool::printMsgError(MsgError);
                std::cout << "  Error in create a relation between" + relation << std::endl;
            }
            else
                std::cout << "Relation Record between" + relation + "created successfully" << std::endl;
        }

        closeDB();
    }

    void SQLite::CreateRelations(std::string &idNode, std::vector<std::string> &nodes_relations)
    {
        std::string idNode_R;
        for (auto &node_r : nodes_relations)
        {
            CreateNodo(node_r, idNode_R);
            CreateRelation(idNode, idNode_R);
        }
    }

    //----------------R:Read-------------------------
    std::vector<std::pair<std::string,std::map<std::string,std::string>>> SQLite::Read(msg::ReadNodePacket &packetRead)
    {
        //std::vector<std::string> neighbours;
        std::vector<std::pair<std::string,std::map<std::string,std::string>>> neighbours;
        std::pair<std::string,std::map<std::string,std::string>> neighbour;
        std::map<std::string,std::string> attributes;
        std::string idNode;
        
        
        if (!(packetRead.depth))
        {
            //std::cout<< "Leaf:"<<packetRead.nodeType <<std::endl;
            if (packetRead.nodeType == msg::ReadNodePacket::Class::Leaf)
            {
                //Controlar la ciclos (pero ;v no guardamos el camino)
                if (existNodo(packetRead.nodeId, idNode))
                {
                    std::vector<bool> recordsFound;
                    tool::Records records;
                    if (packetRead.features.size())
                    {
                        for (auto &feature : packetRead.features)
                        {
                            sql = "SELECT name_attribute, value_attribute from Attribute WHERE idAttribute = '" + idNode + "' AND (";
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

                            existDataBase();
                            rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &records, &MsgError);
                            recordsFound.push_back(records.size());
                        }
                    }

                    bool ans = true; //*Por defecto es true si no hay condicionales
                    records.clear();
                    //--------------Where--------------------
                    if (packetRead.features.size())
                        ans = recordsFound[0];
                    for (int i = 0; packetRead.features.size() != 0 && i < packetRead.features.size() - 1; i++)
                    {
                        if (packetRead.features[i].boolOpId == msg::ReadNodePacket::BooleanOperator::And)
                            ans = ans && recordsFound[i + 1];
                        else if (packetRead.features[i].boolOpId == msg::ReadNodePacket::BooleanOperator::Or)
                            ans = ans || recordsFound[i + 1];
                    }
                    //--------------Fin Where--------------------
                    
                    if (ans){
                         cout<<packetRead.attribsReq<<endl;
                         if (packetRead.attribsReq == msg::ReadNodePacket::QueryMode::Required)
                            {
                                sql = "SELECT name_attribute,value_attribute  FROM Attribute WHERE idAttribute = "+idNode+";";
                                existDataBase();
                                rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &records, &MsgError);
                                closeDB();

                                tool::saveAttributes(records,attributes);
                                neighbour = std::make_pair(packetRead.nodeId,attributes); 

                                neighbours.push_back(neighbour);
                            }

                        else //msg::ReadNodePacket::QueryMode::Required
                            {
                            neighbour = std::make_pair(packetRead.nodeId,attributes);    
                            neighbours.push_back(neighbour);
                            }
                    }
                }
                packetRead.depth--;
                return neighbours;
            }

            //else msg::ReadNodePacket::Class::Internal
            //Devolver con el camino pero no hay camino guardado
        }

        //deep != 0
        else
        {
            if (existNodo(packetRead.nodeId, idNode))
            {
                tool::Records nodes_relations;
                sql = "SELECT Nodo_name_end FROM Relation WHERE Nodo_name_start = '" + idNode + "';";
                existDataBase();
                rc = sqlite3_exec(DB, sql.c_str(), tool::select_callback, &nodes_relations, &MsgError);

                for (auto &row : nodes_relations)
                    for (auto &node_n : row){
                        neighbour = std::make_pair(packetRead.nodeId,attributes);    
                        neighbours.push_back(neighbour);
                    }

                //if(packetRead.nodeType==msg::ReadNodePacket::Class::Internal){}
                //Guardar el Path
            }
            packetRead.depth--;
            return neighbours;
        }
    }

    // ----------------U:Updates-------------------------
    void SQLite::Update(msg::UpdateNodePacket &packetUpdate)
    {
        if (packetUpdate.updateMode == msg::UpdateNodePacket::Mode::Object)
            UpdateValueNodo(packetUpdate.nodeId, packetUpdate.newNodeValue);
        else // Mode::Attribute
            UpdateAttribute(packetUpdate.nodeId, packetUpdate.attrName, packetUpdate.attrValue);
    }

    void SQLite::UpdateValueNodo(std::string &query_value_node, std::string &set_value_node)
    {
        std::string idNode_Old;
        std::string idNode_New;
        if (existNodo(query_value_node, idNode_Old) && !existNodo(set_value_node, idNode_New))
        {
            existDataBase();
            const char *data = "Callback function called";

            sql = "UPDATE Nodo set name_nodo = '" + set_value_node + "' " + "WHERE name_nodo = '" + query_value_node + "';";
            rc = sqlite3_exec(DB, sql.c_str(), tool::print_select_callback, (void *)data, &MsgError);

            if (printError)
            {
                std::string setValues = " [ Old value :" + query_value_node + " ,  New value :" + set_value_node + " ] ";
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cout << "  Error in update name/value in Nodo Table " + setValues << std::endl;
                }
                else
                    std::cout << "Update Successfully of name/value in Nodo Table" + setValues << std::endl;
            }
        }
        else if (printError)
        {
            std::cout << "Impossible Update name/value , there is not Nodo with name [" << query_value_node << "]" << std::endl;
            std::cout << "OR" << std::endl;
            std::cout << "There is one nodo with value [ " << set_value_node << " ] that you want change" << std::endl;
        }
    }

    void SQLite::UpdateAttribute(std::string &query_value_node, std::string &name_attribute, std::string &value_attribute)
    {
        std::string idNode;
        if (existNodo(query_value_node, idNode))
        {
            existDataBase();
            const char *data = "Callback function called";

            //Actulizar atributo
            sql = "UPDATE Attribute set value_attribute = '" + value_attribute + "' " + "WHERE idAttribute = " + idNode + " AND name_attribute = '" + name_attribute + "';";
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
                else if (sqlite3_changes(DB) == 1)
                    std::cout << "Update Successfully of Attribute value " + setValue << std::endl;

                else
                    std::cout << "Not Exist Error in SQL but not exist attribute with name  [ " + name_attribute + " ]" << std::endl;
            }

            closeDB();
        }

        else
            std::cout << "Impossible , there is not one Nodo with name [" << query_value_node << "]" << std::endl;
    }

    //-----------------D:Delete Node o Attributes of Node or Relation
    void SQLite::Delete(msg::DeleteNodePacket &packetDelete)
    {
        if (packetDelete.deleteMode == msg::DeleteNodePacket::Mode::Object)
            DeleteNode(packetDelete.nodeId);
        else if (packetDelete.deleteMode == msg::DeleteNodePacket::Mode::Attribute)
            DeleteValueAttribute(packetDelete.nodeId, packetDelete.targetName);
        else // Mode::Relation
            DeleteRelation(packetDelete.nodeId, packetDelete.targetName);
        closeDB();
    }

    void SQLite::DeleteNode(std::string &value_node)
    {
        std::string idNode;
        if (existNodo(value_node, idNode))
        {
            existDataBase();
            ConstraintForeign();
            sql = "DELETE FROM Nodo WHERE id_Nodo = " + idNode + ";";
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
        }
        else if (printError)
            std::cout << "Not Exist nodo with value [" + value_node + " ]" << std::endl;
    }

    void SQLite::DeleteValueAttribute(std::string &value_node, std::string &name_attribute)
    {
        std::string idNode;
        if (existNodo(value_node, idNode))
        {
            existDataBase();
            sql = "DELETE FROM Attribute WHERE idAttribute = " + idNode + " AND name_attribute = '" + name_attribute + "';";
            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error DELETE Attribute" << std::endl;
                }
                else
                    std::cout << "Record of Attribute " + name_attribute + "in id_Nodo of " + idNode + "deleted Successfully!" << std::endl;
            }
        }
        else if (printError)
            std::cout << "Not Exist nodo with value [" + value_node + " ]" << std::endl;
    }

    void SQLite::DeleteRelation(std::string &value_node, std::string &value_node_Relation)
    {
        std::string idNode;
        std::string idNode_R;
        if (existNodo(value_node, idNode) && existNodo(value_node_Relation, idNode_R))
        {
            existDataBase();
            sql = "DELETE FROM Relation WHERE ( Nodo_name_start = " + idNode + " AND Nodo_name_end = " + idNode_R + " ) OR ( Nodo_name_start = " + idNode_R + " AND Nodo_name_end = " + idNode + " );";
            rc = sqlite3_exec(DB, sql.c_str(), NULL, NULL, &MsgError);

            if (printError)
            {
                if (rc != SQLITE_OK)
                {
                    tool::printMsgError(MsgError);
                    std::cerr << "  Error DELETE Relation" << std::endl;
                }
                else
                    std::cout << "Record of Relation between [ " + idNode + " and " + idNode_R + "deleted Successfully!" << std::endl;
            }
        }
        else if (printError)
        {
            std::cout << "Not Exist nodo with value [" + value_node + " ]" << std::endl;
            std::cout << "OR" << std::endl;
            std::cout << "Not Exist Nodo with name " + value_node_Relation + " For delete relation" << std::endl;
        }
    }
}