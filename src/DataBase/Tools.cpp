#include "DataBase/Tools.hpp"

namespace tool{

    void printMsgError(char *msg)
    {
        std::cout << "SQL error: " << msg << std::endl;
        free(msg);
    }

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

    int print_select_callback(void *flag, int argc, char **argv, char **azColName)
    {

        for (int i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }

        printf("\n");
        return 0;
    }

}