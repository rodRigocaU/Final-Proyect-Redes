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
#include <cstring>
#include <unistd.h>
#include <errno.h>

// C++ stl headers
#include <sstream>
#include <iostream>

#define MAX_DGRAM_SIZE 1000

namespace RDT
{
    class UdpSocket
    {
    private:
    // Network attributes
        int socket_file_descriptor;
        // estas estructuras se usan para la configuración ante el envío de datos
        struct addrinfo hints, *servinfo, *configured_sockaddr;
        // esta estructura se usa para el recibo de datos de parte de un emisor
        struct sockaddr_storage sender_addr;
        socklen_t sender_addr_len;
        
        std::string IP;
        std::string Port;


    public:
        UdpSocket(const std::string &_IP, const std::string &_Port);

        int sendAll(u_char* buffer, int& bytes_sent, bool to_sender);

        int simpleRecv(u_char* buffer, std::string &IP_from, uint16_t &Port_from);

        void* get_in_addr(struct sockaddr *sa);

        uint16_t get_in_port(struct sockaddr *sa);

        ~UdpSocket();

    };

    UdpSocket::UdpSocket(const std::string &_IP, const std::string &_Port)
    {
        IP = _IP;
        Port = _Port;

        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;    // IPv4 or IPv6
        hints.ai_socktype = SOCK_DGRAM; // UDP datagram
        if (IP.empty())
            hints.ai_flags = AI_PASSIVE; // uso mi propia IP, esto es para el caso del servidor

        if ((rv = getaddrinfo(IP.empty() ? NULL : IP.c_str(), Port.c_str(), &hints, &servinfo)) != 0)
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }

        for (configured_sockaddr = servinfo; configured_sockaddr != NULL; configured_sockaddr = configured_sockaddr->ai_next)
        {
            if ((socket_file_descriptor = socket(configured_sockaddr->ai_family, configured_sockaddr->ai_socktype, configured_sockaddr->ai_protocol)) == -1)
            {
                perror("talker: socket");
                continue;
            }

            if (IP.empty())
            {
                if (bind(socket_file_descriptor, configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen) == -1)
                {
                    close(socket_file_descriptor);
                    perror("listener: bind");
                    continue;
                }
            }
            break;
        }

        if (configured_sockaddr == NULL)
        {
            fprintf(stderr, "talker: failed to create socket\n");
            return;
        } 
    }

    UdpSocket::~UdpSocket()
    {
        close(socket_file_descriptor); // cerramos el socket file descriptor
       
        // liberamos memoria de la lista de resultados
        freeaddrinfo(servinfo);
    }


    int UdpSocket::sendAll(u_char* buffer, int& bytes_sent, bool to_sender)
    {
        int total = 0;        // cuantos bytes hemos enviado
        int bytes_left = MAX_DGRAM_SIZE; // cuantos nos falta por enviar
        int n;
        while (total < MAX_DGRAM_SIZE)
        {
            // si el paquete es para el host que nos envió un mensaje
            // o si es para el socket que tenemos configurado 
            if(to_sender)
                n = sendto(socket_file_descriptor, buffer + total, bytes_left, 0,
                           (struct sockaddr *)&sender_addr, sizeof(struct sockaddr_storage));
            else
                n = sendto(socket_file_descriptor, buffer + total, bytes_left, 0,
                           configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen);

            if (n == -1) break;
            total += n;
            bytes_left -= n;
        }
        bytes_sent = total; // la cantidad de bytes enviados realmente
        return n == -1 ? -1 : 0; // return -1 cuano falla, 0 cuando es exitoso
    }

    int UdpSocket::simpleRecv(u_char* buffer, std::string &IP_from, uint16_t &Port_from)
    {
        int n;
        n = recvfrom(socket_file_descriptor, buffer, MAX_DGRAM_SIZE, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_len);

        // 0 cuando el host externo cerró la conexión
        if (n == 0)
        {
            std::cout << "El ordenador de destino se desconectó inesperadamente...\n";
            // this->~UdpSocket(); // antes de retirarnos destruimos el socket
            // exit(1);
        }

        // una vez terminado el recibo de datos
        // obtenemos el IP y puerto del emisor
        char addr_from[INET_ADDRSTRLEN];

        inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr),
                  addr_from, sizeof(addr_from));

        IP_from = addr_from;                                      // seteamos el ip
        Port_from = get_in_port((struct sockaddr *)&sender_addr); // seteamos el puerto

        return n; // MAX_DGRAM_SIZE cuando hay éxito, -1 en el otro caso
    }

    
    void* UdpSocket::get_in_addr(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
        {
            return &(((struct sockaddr_in *)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }

    uint16_t UdpSocket::get_in_port(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
        {
            return ((struct sockaddr_in *)sa)->sin_port;
        }
        return ((struct sockaddr_in6 *)sa)->sin6_port;
    }

}

#endif //UDP_SOCKET_HPP_