#ifndef CLIENT_PROTOCOL_HANDLER_HPP_
#define CLIENT_PROTOCOL_HANDLER_HPP_

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include "App/Tools/InterfacePerformance.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"

namespace app
{

    class Client
    {
    private:
        std::unordered_map<std::string, std::function<bool(void)>> commands;

        bool create();
        bool read();
        bool update();
        bool drop();

        void askAtributes(std::vector<std::string> &data, std::string &number_of_attributes);
        void askRelations(std::vector<std::string> &data, std::string &number_of_relations);
        void askConditions(std::vector<std::string> &data, std::string &number_of_conditions);
        void showHelpOperator();
        void showHelpOperatorLogic();
        void showHelpUpdate();
        void showHelpDrop();

    public:
        Client(const std::string& serverIp, const std::string& remotePort);
        bool setCommand(const std::string& command);
    };

    Client::Client(const std::string& serverIp, const std::string& remotePort)
    {
      commands["spawn"]  = std::bind(&Client::create, this);
      commands["ask"]    = std::bind(&Client::read, this);
      commands["update"] = std::bind(&Client::update, this);
      commands["drop"]   = std::bind(&Client::drop, this);
    }

    bool Client::setCommand(const std::string& command)
    {
      return commands[command]();
    }

    bool Client::create(){
      trlt::CreateNodePacket packet;
      tool::getInput("Ingrese el nombre:", VIOLET);
      std::vector<std::string> data;
      tool::ConsolePrint("Spawn Node", GREEN);
      tool::ConsolePrint("Ingrese el nombre/value del Nodo:", VIOLET);
      std::string name_node;
      std::getline(std::cin, name_node);
      data.push_back(name_node);

        tool::ConsolePrint("Ingrese el numero de atributos:", VIOLET);
        std::string number_of_attributes;
        std::getline(std::cin, number_of_attributes);
        tool::fixToBytes(number_of_attributes, 2);
        data.push_back(number_of_attributes);

        tool::ConsolePrint("Ingrese el numero de relaciones:", VIOLET);
        std::string number_of_relations;
        std::getline(std::cin, number_of_relations);
        tool::fixToBytes(number_of_attributes, 3);
        data.push_back(number_of_relations);

        //Opcional si hay atributos
        askAtributes(data, number_of_attributes);

        //Opcional si hay relaciones
        askRelations(data, number_of_relations);

        //ConstruirMensaje
        return true;
    }

    bool Client::read()
    {
        std::vector<std::string> data;
        tool::ConsolePrint("Leer los datos de un nodo: ", GREEN);


        tool::ConsolePrint("Ingrese el nombre/value del Nodo:", VIOLET);
        std::string query_node;
        std::getline(std::cin, query_node);
        data.push_back(query_node);
        

        tool::ConsolePrint("Ingrese la profundidad:", VIOLET);
        std::string deep;
        std::getline(std::cin, deep);
        data.push_back(deep);  

        tool::ConsolePrint("¿Quieres considerar todo el recorrido o solo el ultimo nodo?", VIOLET); //Solo poner 0 para off y 1 para on
        std::string leaf;
        std::getline(std::cin, leaf);
        data.push_back(leaf); 


        tool::ConsolePrint("¿Quieres extraer los atributos ?", VIOLET); //Solo poner 0 para off y 1 para on
        std::string attributes;
        std::getline(std::cin, attributes);
        data.push_back(attributes); 


        tool::ConsolePrint("Ingrese el numero de  condiciones:", VIOLET);
        std::string number_of_conditions;
        std::getline(std::cin, number_of_conditions);
        data.push_back(number_of_conditions);

        //Opcional si hay condiciones
        askConditions(data,number_of_conditions);
      

        //ConstruirMensaje


    }

    bool Client::update()
    {
        // std::vector<std::string> data;
        // // tool::ConsolePrint("Actualizar los datos de un nodo: ", GREEN);
        // tool::ConsolePrint("Actualizar", GREEN);

        // showHelpUpdate();
        // tool::ConsolePrint("¿Que deseas actulizar valor del Nodo o atributo del Nodo?:", VIOLET);
        // std::string node_or_attribute; 
        // std::getline(std::cin, node_or_attribute);
        
        
        // tool::ConsolePrint("Ingrese el value/name del Nodo:", VIOLET);
        // std::string query_value_node; 
        // std::getline(std::cin, query_value_node);
        
        // //Actulizar Atributos
        // if (node_or_attribute==0)
        // {
        //     tool::ConsolePrint("Ingrese el nombre del atributo para la actulizacion: ", GREEN);
        //     std::string query_value_attribute_size; 
        //     std::getline(std::cin, query_value_attribute_size; );
            

        //     tool::ConsolePrint("Ingrese el nuevo valor del atributo para la actulizacion: ", GREEN);
        //     std::string set_value_attribute_size; 
        //     std::getline(std::cin, set_value_attribute_size);

        //     //ConstruirMensaje
        // }

        // //Actualizar valor/name Nodo
        // else if (node_or_attribute==0)
        // {
        //     tool::ConsolePrint("Ingrese el nuevo value/name del Nodo:", VIOLET);
        //     std::string set_value_node; 
        //     std::getline(std::cin, set_value_node);

        //     //ConstruirMensaje
        // }
        

    }
    bool Client::drop()
    {
        // std::vector<std::string> data;
        // // tool::ConsolePrint("Borrar los datos de un nodo: ", GREEN);
        // tool::ConsolePrint("Borrar", GREEN);

        // showHelpDrop();
        // tool::ConsolePrint("¿Que deseas borrar Nodo , Atributo o Relación?:", VIOLET);
        // std::string node_or_attribute; 
        // std::getline(std::cin, node_or_attribute);

        // tool::ConsolePrint("Ingrese el value/name del Nodo:", VIOLET);
        // std::string query_value_node; 
        // std::getline(std::cin, query_value_node);


        // std::string query_value_attribute_o_R_size;
        // // Aqui estoy fly mano ni idea como dividio okydoky ;v
        // if (node_or_attribute=="1")
        // {
        //     //ConstruirMensaje
        //     //No olvidar que destruyes el nodo junto a todas sus relaciones que se estaban
        // }
        
        // else if (node_or_attribute=="2")
        // {
        //     tool::ConsolePrint("Ingrese el nombre del atributo:", VIOLET);
        //     std::getline(std::cin, query_value_attribute_o_R_size);

        //     //ConstruirMensaje
        // }
        
        // else if (node_or_attribute=="3")
        // {
        //     tool::ConsolePrint("Ingrese el value/name del Nodo relacionado:", VIOLET);
        //     std::getline(std::cin, query_value_attribute_o_R_size);

        //     //ConstruirMensaje
        // }  
    }


    // ----------------------------Opcionales----------------------------
    void Client::askAtributes(std::vector<std::string> &data, std::string &number_of_attributes)
    {
        // size_t number_attributes = tool::stringToSize_t(number_of_attributes);

        // std::string name_attribute;
        // std::string value_attribute;

        // while (number_attributes != 0)
        // {
        //     tool::ConsolePrint("Ingrese el nombre del atributo: ", GREEN);
        //     std::getline(std::cin, name_attribute);
        //     data.push_back(name_attribute);

        //     tool::ConsolePrint("Ingrese el valor del atributo: ", GREEN);
        //     std::getline(std::cin, value_attribute);
        //     data.push_back(value_attribute);

        //     number_attributes--;
        // }
    }

    void Client::askRelations(std::vector<std::string> &data, std::string &number_of_relations)
    {
        // size_t number_relations = tool::stringToSize_t(number_of_relations);

        // std::string node_relations;

        // while (number_relations != 0)
        // {
        //     tool::ConsolePrint("Ingrese el nombre del nodo a relacionarse: ", GREEN);
        //     std::getline(std::cin, node_relations);
        //     data.push_back(node_relations);

        //     number_relations--;
        // }
    }

     void Client::askConditions(std::vector<std::string> &data, std::string &number_of_conditions){
        
        // size_t number_conditions = tool::stringToSize_t(number_of_conditions);

        // std::string query_name_attribute_size; //edad
        // std::string operador; // 1=, 2>, 3<,4like 
        // std::string query_value_attribute; //25
        // std::string is_and; // 1->AND 0->OR
        
        // while (number_conditions != 0)
        // {
        //     tool::ConsolePrint("Ingrese el nombre del atributo para la condición: ", GREEN);
        //     std::getline(std::cin, query_name_attribute_size);
            
        //     showHelpOperator();
        //     tool::ConsolePrint("Ingrese el operador para la condición: ", GREEN);
        //     std::getline(std::cin, operador);
            
        //     tool::ConsolePrint("Ingrese el valor del atributo para la condición: ", GREEN);
        //     std::getline(std::cin, query_value_attribute);
            
        //     showHelpOperatorLogic();
        //     tool::ConsolePrint("¿Con que operador (AND,OR) deseas que se filtren los resultados?: ", GREEN);
        //     std::getline(std::cin, query_value_attribute);

        //     number_conditions--;
        // }
     }

     void showHelpOperator()
     {
          tool::ConsolePrint("CONDICIONALES: ", RED_BG);
          tool::ConsolePrint("PRESIONAR 1 PARA EJECUTAR EL OPERADOR: = ", GREEN);
          tool::ConsolePrint("PRESIONAR 2 PARA EJECUTAR EL OPERADOR: > ", GREEN);
          tool::ConsolePrint("PRESIONAR 3 PARA EJECUTAR EL OPERADOR: < ", GREEN);
          tool::ConsolePrint("PRESIONAR 4 PARA EJECUTAR EL OPERADOR: LIKE (CUANDO SON PARECIDOS AL VALOR DE BUSQUEDA)", GREEN);
     }

     void showHelpOperatorLogic()
     {
          tool::ConsolePrint("PRESIONAR 0 (AND) o 1 (OR) PARA FILTRAR LOS RESULTADOS ", GREEN);
     }

     void showHelpUpdate()
     {
         tool::ConsolePrint("PRESIONAR 0 (PARA MODIFICAR AL NODO) o 1 (PARA MODIFICAR EL ATRIBUTO) PARA FILTRAR LOS RESULTADOS ", GREEN);
     }
      void showHelpDrop()
     {
         tool::ConsolePrint("PRESIONAR 1 (PARA BORRAR EL NODO) o  2 (PARA BORRAR EL ATRIBUTO) o 3 (PARA BORRAR EL ATRIBUTO) PARA FILTRAR LOS RESULTADOS ", GREEN);
     }

}

#endif //CLIENT_PROTOCOL_HANDLER_HPP_