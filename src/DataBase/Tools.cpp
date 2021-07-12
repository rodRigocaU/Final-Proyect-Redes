#include "DataBase/Tools.hpp"

namespace tool
{

    void printMsgError(char *msg)
    {
        std::cout << "SQL error: " << msg << std::endl;
        sqlite3_free(msg);
    }

    int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
    {
        Records *records = static_cast<Records *>(p_data);
        try
        {
            records->emplace_back(p_fields, p_fields + num_fields);
        }
        catch (...)
        {
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

    int print_select_callback(void *flag, int argc, char **argv, char **azColName)
    {

        for (int i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }

        printf("\n");
        return 0;
    }

    void printNode(std::string &name_node)
    {
        std::cout << "Nodo : " << name_node << std::endl;
    }

    void printRecords(Records records)
    {
        for (auto &i : records)
        {
            for (auto &j : i)
                printElement(j, nameWidth);
            std::cout << std::endl;
        }
    }

    bool isTxt(std::string &nameAttribute){
        if (nameAttribute.size() > 4)
        {
            std::string extension = nameAttribute.substr(nameAttribute.size()-4,4);
            if  (extension==".txt")
                return true;
        }
        return false;
    }

    bool isImg(std::string &nameAttribute){
        if (nameAttribute.size() > 4)
        {
            std::string extension = nameAttribute.substr(nameAttribute.size()-4,4);
            if  (extension==".png")
                return true;
            else if (extension==".jng")
                return true;
        }
        return false;
    }


    template <typename T>
    void printElement(T t, const int &width)
    {
        std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
    }
}