#ifndef UDP_SOCKET_HPP_
#define UDP_SOCKET_HPP_
// networking headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

// C headers
#include <string.h>
#include <unistd.h>
#include <errno.h>

// C++ stl headers
#include <sstream>
#include <iostream>

#define MAX_DGRAM_SIZE 1000

namespace RDT {
    
    class RDT_Components
    {
        public:
        static void fillZeroes(std::string& s){
            if(s.length() > MAX_DGRAM_SIZE) return;
            s.append(MAX_DGRAM_SIZE - s.length(), '0');
        }
    };
    
    class UdpSocket {
        public:
            UdpSocket(const std::string& _IP, const std::string & _Port);

            bool send(const std::string& message);

            int secure_send(std::string& message);

            std::string receive();

            ~UdpSocket();

        private:

            int socket_file_descriptor;
            
            // estas estructuras se usan para la configuración ante el envío de datos
            struct addrinfo hints, *servinfo, *configured_sockaddr;

            // esta estructura se usa para el recibo de datos de parte de un emisor
            struct sockaddr_storage sender_addr;
            socklen_t sender_addr_len;

            // El IP y puerto de nuestro socket
            std::string IP;
            std::string Port;
    };
  
    UdpSocket::UdpSocket(const std::string & _IP, const std::string & _Port){
        IP = _IP;
        Port = _Port;

        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
        hints.ai_socktype = SOCK_DGRAM; // UDP datagram
        if(IP.empty())
            hints.ai_flags = AI_PASSIVE; // uso mi propia IP, esto es para el caso del servidor


        // en caso de que no se envíe ninguna IP, eso significa que el socket se ubicará en el
        // lado del servidor
        if ((rv = getaddrinfo(IP.empty()? NULL : IP.c_str(), Port.c_str(), &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }

        // loop through all the results and make a socket
        for(configured_sockaddr = servinfo; configured_sockaddr != NULL; configured_sockaddr = configured_sockaddr->ai_next) {
            if ((socket_file_descriptor = socket(configured_sockaddr->ai_family, configured_sockaddr->ai_socktype, configured_sockaddr->ai_protocol)) == -1) {
                perror("talker: socket");
                continue;
            }

            // indicamos que es un socket pasivo, osea el del servidor
            if(IP.empty()){
                if (bind(socket_file_descriptor, configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen) == -1) {
                    close(socket_file_descriptor);
                    perror("listener: bind");
                    continue;
                }
            }

            break;
        }

        // fallamos al crear el socket
        if (configured_sockaddr == NULL) {
            fprintf(stderr, "talker: failed to create socket\n");
            return;
        }

    }

    UdpSocket::~UdpSocket()
    {
        // liberamos la memoria del socket al cual apunta nuestro socket
        freeaddrinfo(configured_sockaddr);
        close(socket_file_descriptor);
    }

    bool UdpSocket::send(const std::string& message)
    {
        int chunk_size = MAX_DGRAM_SIZE;
        std::string message_2send = message;

        RDT_Components::fillZeroes(message_2send);

        if(!secure_send(message_2send)) return true;

        return false;
    }

    // el mensaje siempre tiene tamaño de MAX_DGRAM_SIZE
    int UdpSocket::secure_send(std::string& message) {
        int bytes_sent = 0, chunk_size = MAX_DGRAM_SIZE;
        
        do{
            chunk_size -= bytes_sent;
            message = message.substr(bytes_sent, chunk_size);

            bytes_sent = sendto(socket_file_descriptor, message.c_str(), chunk_size, 0, 
                          configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen);
            
            if (bytes_sent < 0)
            {    
                perror("talker: sendto");
                return -1; // retornamos -1 como error
            }
        } while(bytes_sent < chunk_size);
        
        return 0; // retornamso 0 como éxito al enviar
    }


    std::string UdpSocket::receive()
    {
        std::string recv_message = ""; // received message
        char buffer[MAX_DGRAM_SIZE + 1];

        int bytes_received = 0, chunk_size = MAX_DGRAM_SIZE;
        do{
            bytes_received = recvfrom(socket_file_descriptor, buffer, chunk_size, 0,
                                      (struct sockaddr *)&sender_addr, &sender_addr_len);

            buffer[bytes_received] = '\0';

            if(bytes_received < 0)
            {
                perror("recvfrom");
                return "-1"; // error string
            }

            recv_message += buffer;

            chunk_size -= bytes_received;

        } while(chunk_size > 0); // mientras todavía  tengamos algo que leer


        return recv_message; // return received message on success
    }

}


#endif//UDP_SOCKET_HPP_