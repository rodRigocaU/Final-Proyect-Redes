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

    class RDT_Components
    {
    public:
        static void fillZeroes(std::string &s)
        {
            if (s.length() > MAX_DGRAM_SIZE)
                return;
            s.append(MAX_DGRAM_SIZE - s.length(), '0');
        }

        // es sabido que todos los paquetes tienen el tamaño entre el dominio [0,998]
        static void make_pkt(const std::string &s, char buff[])
        {
            short pkt_size = s.length();
            // subdividimos el tamaño del paquete en dos enteros de tipo "byte"
            // lbyte son los 8 bits mas significativos, rbyte los 8 bits menos significativos
            unsigned char lbyte = (pkt_size >> 8), rbyte = static_cast<unsigned char>(pkt_size);

            // no hay problema ya que 998(10) = 0000 0011 1110 0110(2)
            // y en caso de que pkt_size = 0000 0000 **** ****(2), el primer bloque
            // sería nulo, lo cual trae problemas a la hora de leer los datos (lee nulo)
            // entonces simplemente el primer byte es 1111 1111
            if (lbyte == 0)
                lbyte = 255;

            buff[0] = lbyte;
            buff[1] = rbyte;

            // en buff iniciamos desde la posición 2
            for (int i = 0; i < pkt_size; ++i)
                buff[i + 2] = s[i];

            // rellenamos de 0's las posiciones restantes, si sobrase espacio
            for (int i = 2 + pkt_size; i < MAX_DGRAM_SIZE; ++i)
                buff[i] = '0';
        }

        // recibiremos un buffer de tamaño MAX_DGRAM_SIZE = 1000
        static std::string recv_pkt(unsigned char buff[])
        {
            int decoded_size = (buff[0] == 255) ? buff[1] : (buff[0] << 8) | buff[1];
            std::cout << "decoded size: " << decoded_size << "\n";

            std::string received_packet = "";

            for (int i = 2; i < 2 + decoded_size; ++i)
                received_packet += buff[i];

            return received_packet;
        }
    };

    class UdpSocket
    {
    public:
        UdpSocket(const std::string &_IP, const std::string &_Port);

        bool send(const std::string &message);

        int secure_send(char *buf, int *len);

        int receive(std::string &recv_message, std::string &IP_from, uint16_t &Port_from);

        // get sockaddr, IPv4 or IPv6:
        void* get_in_addr(struct sockaddr *sa);

        uint16_t get_in_port(struct sockaddr *sa);

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

        // en caso de que no se envíe ninguna IP, eso significa que el socket se ubicará en el
        // lado del servidor
        if ((rv = getaddrinfo(IP.empty() ? NULL : IP.c_str(), Port.c_str(), &hints, &servinfo)) != 0)
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return;
        }

        // loop through all the results and make a socket
        for (configured_sockaddr = servinfo; configured_sockaddr != NULL; configured_sockaddr = configured_sockaddr->ai_next)
        {
            if ((socket_file_descriptor = socket(configured_sockaddr->ai_family, configured_sockaddr->ai_socktype, configured_sockaddr->ai_protocol)) == -1)
            {
                perror("talker: socket");
                continue;
            }

            // indicamos que es un socket pasivo, osea el del servidor
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

        // fallamos al crear el socket
        if (configured_sockaddr == NULL)
        {
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

    bool UdpSocket::send(const std::string &message)
    {
        char buffer[MAX_DGRAM_SIZE];

        RDT_Components::make_pkt(message, buffer);

        int len = MAX_DGRAM_SIZE;
        if (secure_send(buffer, &len) == -1)
        {
            perror("secure_send");
            std::cout << "solo se pudo enviar " << len << " bytes de información\n";
        }

        return false;
    }

    // el mensaje siempre tiene tamaño de MAX_DGRAM_SIZE
    int UdpSocket::secure_send(char *buf, int *len)
    {
        int total = 0, bytes_left = *len, n;

        while (total < *len)
        {
            n = sendto(socket_file_descriptor, buf + total, bytes_left, 0,
                       configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen);
            if (n == -1)
                break;
            total += n;
            bytes_left -= n;
        }

        *len = total;

        return (n == -1) ? -1 : 0; // retornamos 0 como éxito al enviar, -1 como falla
    }

    int UdpSocket::receive(std::string &recv_message, std::string &IP_from, uint16_t &Port_from)
    {
        unsigned char buffer[MAX_DGRAM_SIZE + 1];

        int n;
        n = recvfrom(socket_file_descriptor, buffer, MAX_DGRAM_SIZE, 0,
                     (struct sockaddr *)&sender_addr, &sender_addr_len);

        if (n == -1)
        {
            perror("receive");
            std::cout << "error al leer los datos\n";
        }

        buffer[n] = '\0';

        char addr_from[INET_ADDRSTRLEN];

        inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr),
                  addr_from, sizeof(addr_from));

        IP_from = addr_from;
        Port_from = get_in_port((struct sockaddr *)&sender_addr);

        recv_message = RDT_Components::recv_pkt(buffer);
        return n; // 0: success, -1: failure
    }

    // get sockaddr, IPv4 or IPv6:
    void* UdpSocket::get_in_addr(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
        {
            return &(((struct sockaddr_in *)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }

    // get sockport, IPv4 or IPv6:
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