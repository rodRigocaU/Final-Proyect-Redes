#ifndef UDP_SOCKET_RDT_HPP_
#define UDP_SOCKET_RDT_HPP_
// networking headers
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
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
#include <thread>
#include <chrono>
#include <future>

#define MAX_DGRAM_SIZE 1000

namespace RDT
{
    class RDTCoder
    {
        uint8_t alt_bit; // (alternating bit) solo toma valores en {1,2}
    public:
        RDTCoder()
        {
            alt_bit = 1;
        }

        uint8_t getAltBit() const { return alt_bit; }
        void setAltBit(uint8_t new_alt_bit) { alt_bit = new_alt_bit; }

        void switch_alt_bit()
        {
            alt_bit = (alt_bit == 1) ? 2 : 1;
        }

        // entradas:
        // s: un string de tamaño entre [1, 996] que es el mensaje
        // buff: un array de caracteres, que es el paquete
        void make_pkt(const std::string &s, u_char (&buff)[MAX_DGRAM_SIZE])
        {
            uint16_t pkt_size = s.length();
            uint16_t header_size = 4;
            // 1) primero asignamos al paquete su bit alternante
            buff[0] = static_cast<u_char>(alt_bit);

            // 2) luego calculamos el checksum del paquete y lo asignamos a la cabecera
            //    del paquete
            u_char checksum = 0;
            for (int i = 0; i < pkt_size; ++i)
            {
                if (checksum + static_cast<u_char>(s[i]) > 255)
                {
                    checksum += static_cast<u_char>(s[i]); // esta suma causará overflow
                    ++checksum;                            // sumamos el bit resultante del overflow
                    continue;
                }
                // si no causa overflow, simplemente sumamos
                checksum += static_cast<u_char>(s[i]);
            }
            // el checksum será el complemento a 1 de la suma total
            checksum = 255 - checksum;
            buff[1] = checksum;

            // 3) guardamos el tamaño del paquete en la cabecera
            // subdividimos el tamaño del paquete en dos enteros de tipo "byte"
            // lbyte son los 8 bits mas significativos, rbyte los 8 bits menos significativos
            u_char lbyte = static_cast<u_char>((pkt_size >> 8)), rbyte = static_cast<u_char>(pkt_size);

            // no hay problema ya que 996(10) = 0000 0011 1110 0100(2)
            // y en caso de que pkt_size = 0000 0000 **** ****(2), el primer bloque
            // sería nulo, lo cual trae problemas a la hora de leer los datos (lee nulo)
            // entonces simplemente el primer byte es 1111 1111(2) = 255(10)
            if (lbyte == 0)
                lbyte = 255;
            buff[2] = lbyte, buff[3] = rbyte;

            // 4) copiamos el contenido del mensaje en el paquete
            // en buff iniciamos desde la posición donde acaba el header
            for (uint16_t i = 0; i < pkt_size; ++i)
                buff[i + header_size] = s[i];

            // rellenamos de 0's las posiciones restantes, si sobrase espacio
            for (uint16_t i = header_size + pkt_size; i < MAX_DGRAM_SIZE; ++i)
                buff[i] = '0';
        }

        // retorna el ack recibido de un mensaje enviado, solo puede tomar dos valores {1,2}
        static uint8_t getACK(u_char buff[])
        {
            uint16_t header_size = 4;
            return static_cast<uint8_t>(buff[header_size]);
        }

        ~RDTCoder() {}
    };

    class RDTDecoder
    {
    public:
        bool corrupted;              // checks if there are a corrupted message
        std::string decoded_message; // stores a decoded message
        uint8_t alt_bit;             // especifica el tipo de ack {1,2}

        RDTDecoder()
        {
            corrupted = 0;           // no hay mensaje corrupto, porque todavía no hemos recibido el mensaje
            decoded_message.clear(); // vacío hasta que recibamos un mensaje
            alt_bit = 1;             // inicia con 1
        }

        void switch_alt_bit()
        {
            alt_bit = (alt_bit == 1) ? 2 : 1;
        }
        // recibiremos un buffer de tamaño MAX_DGRAM_SIZE = 1000
        // y retornaremos su contenido, también evaluaremos si el paquete llegó
        // corrupto configurando una variable booleana corrupt, true, si el paquete
        // está corrupto false si no lo está
        void recv_pkt(u_char buff[])
        {
            uint16_t header_size = 4;
            int decoded_size = (buff[2] == 255) ? buff[3] : (buff[2] << 8) | buff[3];
            std::cout << "decoded_size: " << decoded_size << std::endl;
            decoded_message.clear();

            u_char checksum = 0;
            for (uint16_t i = header_size; i < header_size + decoded_size; ++i)
            {
                decoded_message += buff[i];
                // calculamos la suma de todos los caracteres
                if (checksum + buff[i] > 255)
                {
                    checksum += buff[i]; // esta suma causará overflow
                    ++checksum;          // sumamos el bit resultante del overflow
                    continue;
                }
                // si no causa overflow, simplemente sumamos
                checksum += buff[i];
            }
            checksum += buff[1]; // en el byte de posición 1 del paquete
                                 // se encuentra el complemento a 1 del checksum

            alt_bit = static_cast<uint8_t>(buff[0]); // el identificador se encuentra en la posición 0
            corrupted = (checksum != 255);           // si los 8 bits están encendidos, no hubo error
        }

        ~RDTDecoder() {}
    };

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
        // este es nuestro timer, para verificar que no hemos tenido package loss
        struct pollfd ufds[1];
        
        
        // El IP, puerto de nuestro socket, y el bit alternante del protocolo
        std::string IP;
        std::string Port;
        // cada socket tiene un codificador y decodificador al momento de realizar
        // sus envíos y recibir información
        RDTCoder coder;
        RDTDecoder decoder;

        // RDT send and receive functions
        int sendAll(u_char* buffer, int& bytes_sent, bool to_sender);
        int simpleRecv(u_char* buffer);
        int secureSend(u_char* buffer, uint8_t expected_alt_bit);
        int secureRecv(u_char* buffer, uint8_t expected_alt_bit);
        void *get_in_addr(struct sockaddr *sa);
        uint16_t get_in_port(struct sockaddr *sa);        
    public:

        UdpSocket(const std::string &_IP, const std::string &_Port);

        bool send(const std::string &message);

        int receive(std::string &recv_message, std::string &IP_from, uint16_t &Port_from);

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

        // liberamos la memoria de la lista
        freeaddrinfo(servinfo);

        // configuramos nuestro timer una vez ha sido creado el socket
        ufds[0].fd = socket_file_descriptor;
        ufds[0].events = POLLIN; // para lectura de mensajes

    }

    UdpSocket::~UdpSocket()
    {
        // cerramos nuestro sockets
        close(socket_file_descriptor);
    }

    /* MÉTODOS PRIVADOS */
    // ENTRADAS:
    // buffer: el array donde inicia el mensaje
    // bytes_sent: la cantidad de bytes enviados realmente
    // to_sender: true si se envía al host que nos envía un mensaje, false si lo enviamos
    //            al host que tenemos configurado en nuestro socket
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

    // ENTRADAS:
    // buffer: el array de caracteres en donde se guardará los datos obtenidos
    int UdpSocket::simpleRecv(u_char* buffer)
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

        return n; // MAX_DGRAM_SIZE cuando hay éxito, -1 en el otro caso
    }

    // ENTRADAS:
    // buffer: el paquete de 1000 bytes que enviaremos
    // expected_alt_bit: el id de paquete del cual esperamos obtener un ack con el mismo id
    int UdpSocket::secureSend(u_char* buffer, uint8_t expected_alt_bit)
    {
        int bytes_sent = 0; // la cantidad de bytes enviados
        bool reached_correct_to_dest = false;

        int milli = 200; // esto tendrá que ser la función para calcular el RTT
        u_char recv_buffer[MAX_DGRAM_SIZE];

        int ret;

        // mientras el mensaje no haya llegado correctamente a su destino
        // seguiremos enviando el mensaje, tres son los casos para un error:
        // 1. package timeout: el paquete llegó tarde al receptor, se invocó timeout
        // 2. package loss: el paquete se perdió, se invocó timeout
        // 3. ack timeout: el ack llegó tarde, se invocó timeout
        do
        {
            reached_correct_to_dest = false;
            // enviamos el paquete al host al cual está conectado nuestro socket
            if (sendAll(buffer, bytes_sent, false) == -1)
            {
                perror("sendAll");
                std::cout << "no pudimos enviar todos los bytes del paquete :(" << std::endl;
                std::cout << "solo se pudo enviar: " << bytes_sent << " bytes." << std::endl;
                return -1; // retornamos error
            }

            std::cout << "debug" << std::endl;

            // llamamos a la función poll, que esperará una cantidad milli de milisegundos
            // hasta que algún mensaje llegue a nuestro socket
            ret = poll(ufds, 1, milli);
            if (ret == -1) // hubo un error
            {
                perror("poll");
                return -1; // retornamos error
            }
            else if (ret == 0) // hubo timeout
            {
                continue; // volvemos a enviar el paquete
            }
            else
            {
                if (ufds[0].revents & POLLIN)
                {
                    // el mensaje llegó correctamente
                    // recibimos el ack y lo guardamos en recv_buffer
                    if (simpleRecv(recv_buffer) == -1)
                    {
                        perror("simpleRecv");
                        std::cout << "no pudimos recibir todos los bytes del paquete :(" << std::endl;
                        return -1; // error en el envío de datos
                    }
                    // decodificamos el mensaje
                    decoder.recv_pkt(recv_buffer);
                    std::cout << "ack: " << decoder.decoded_message << std::endl;

                    // si el ack llegó corrupto, o no tiene el id esperado, volvemos a enviar
                    // el paquete
                    if (decoder.corrupted || decoder.alt_bit != expected_alt_bit)
                        continue;

                    // si todo el ack llegó correctamente, el paquete llegó correctamente a su
                    // destino
                    reached_correct_to_dest = true;
                }
            }
        } while (!reached_correct_to_dest);
        return 0; // retornamos 0 como éxito al enviar
    }

    // ENTRADAS:
    // buffer: el paquete de 1000 bytes, en el que se almacenará el paquete recibido
    // expected_alt_bit: el id esperado para recibir el mensaje
    int UdpSocket::secureRecv(u_char* buffer, uint8_t expected_alt_bit)
    {
        // un buffer para reenviar los acknowledgments
        u_char ack_buffer[MAX_DGRAM_SIZE];
        // n es la cantidad de bytes leídos y enviados
        int n; 
        do
        {
            // hubo errores al recibir el paquete
            if ((n = simpleRecv(buffer)) == -1){
                perror("receive");
                return -1; // falló el recibo de datos
            }
            // decodificamos el paquete
            decoder.recv_pkt(buffer);

            // si el mensaje llegó corrupto, o no tiene el id esperado, debemos enviar un NAK
            if (decoder.corrupted || decoder.alt_bit != expected_alt_bit)
            {
                // si id de coder es igual al id esperado, debemos hacer que difieran
                if (coder.getAltBit() == expected_alt_bit) coder.switch_alt_bit();
                coder.make_pkt("NAK", ack_buffer);
            }
            else // si el mensaje llegó correcto, reenviamos el ack
            {
                coder.make_pkt("ACK", ack_buffer);
            }

            // enviamos el ack o nak, al host que nos envió el mensaje
            if (sendAll(ack_buffer, n, true) == -1)
            {
                perror("sendAll");
                std::cout << "no pudimos enviar todos los bytes del paquete :(" << std::endl;
                std::cout << "solo se pudo enviar: " << n << " bytes." << std::endl;
                return -1; // falló el recibo de datos
            }

        // mientras el mensaje llegue corrupto o sin el id esperado, debemos seguir
        // esperando por el correcto mensaje
        } while (decoder.corrupted || decoder.alt_bit != expected_alt_bit);
        
        return 0; // el recibo de datos fué exitoso
    }

    // DESCRIPCIÓN:
    // get sockaddr, IPv4 or IPv6:
    // ENTRADAS:
    // sa: puntero a una estructura sockaddr
    void *UdpSocket::get_in_addr(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
        {
            return &(((struct sockaddr_in *)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }

    // DESCRIPCIÓN:
    // get sockport, IPv4 or IPv6:
    // ENTRADAS:
    // sa: puntero a un estructura sockaddr
    uint16_t UdpSocket::get_in_port(struct sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
        {
            return ((struct sockaddr_in *)sa)->sin_port;
        }
        return ((struct sockaddr_in6 *)sa)->sin6_port;
    }


    /* MÉTODOS PÚBLICOS */
    // retorna verdadero si el mensaje se envió correctamente, de lo contrario falso
    bool UdpSocket::send(const std::string &message)
    {
        // al principio el codificador debe tener el id en 1
        if(coder.getAltBit() == 2) coder.switch_alt_bit();
        
        // un buffer en el que se almacenarán los paquetes
        u_char buffer[MAX_DGRAM_SIZE];

        // la cantidad de paquetes es: tamaño_del_mensaje / 996, porque el header ocupa 4 bytes
        // y en caso de que sobren datos de la división, añadimos un paquete extra
        uint32_t n_packets = (message.length() % 996) ? (message.length() / 996) + 1 
                             : (message.length() / 996);

        // primero hacemos un paquete conteniendo la cantidad de paquetes a enviar
        coder.make_pkt(std::to_string(n_packets), buffer);
        

        int times_to_send = 5;
        // enviamos la cantidad de paquetes a ser transmitidos, y cuando suceda algún
        // error seguimos invocando la función por una cantidad razonable de veces,
        // si persiste el error, imprimimos un error al cliente
        int times = 0;
        while(times < times_to_send && secureSend(buffer, coder.getAltBit()) == -1)
            ++times;

        if(times == times_to_send) // significa que el error persistió
        {
            std::cout << "error fatal en el programa :( cerrando el programa...\n";
            std::cout << "pruebe mas tarde...\n";
            exit(1); // el programa falló
        }

        coder.switch_alt_bit(); // intercambiamos el bit alternante

        // ahora el receptor está listo para recibir todos los paquetes restantes, y concatenarlos
        for (int i = 0, j = 0; i < n_packets; ++i, j += 996)
        {
            // hacemos un paquete con un trozo de 996 caracteres del mensaje original
            coder.make_pkt(message.substr(j, 996), buffer);
            times = 0;
            // y enviamos el trozo de mensaje
            while (times < times_to_send && secureSend(buffer, coder.getAltBit()) == -1)
                ++times;

            if(times == times_to_send)
            {
                std::cout << "error fatal en el programa :( cerrando el programa...\n";
                std::cout << "pruebe mas tarde...\n";
                exit(1); // el programa falló
            }
            // para que la siguiente llamada a make_pkt, genere un paquete con un id diferente
            coder.switch_alt_bit();
        }

        return true;
    }

    // obtiene una cadena de caracteres de cualquier tamaño
    int UdpSocket::receive(std::string &recv_message, std::string &IP_from, uint16_t &Port_from)
    {
        // limpiamos el string del mensaje a recibir
        recv_message.clear();
        
        // El id del paquete esperado al inicio es 1, luego va alternando entre {1,2}
        if(decoder.alt_bit == 2) decoder.switch_alt_bit();
        uint8_t expected_alt_bit = decoder.alt_bit;
        
        // un buffer para los mensajes recibidos
        u_char recv_buffer[MAX_DGRAM_SIZE];

        // 1) recibimos la cantidad de paquetes a ser transmitidos
        if(secureRecv(recv_buffer, expected_alt_bit) == -1)
            return -1; // error al recibir datos

        // obtenemos la cantidad de paquetes a recibir
        int n_packets = std::stoi(decoder.decoded_message);
        // cambiamos el id esperado
        decoder.switch_alt_bit(); 
        expected_alt_bit = decoder.alt_bit;

        // empezaremos a recibir todos los paquetes, y los concatenaremos al 
        // string pasado por referencia
        for (int i = 0; i < n_packets; ++i)
        {
            if(secureRecv(recv_buffer, expected_alt_bit) == -1)
                return -1; // error al recibir datos

            // añadimos el mensaje recibido a nuestro string
            recv_message += decoder.decoded_message;
            // cambiamos el id esperado
            decoder.switch_alt_bit();
            expected_alt_bit = decoder.alt_bit;
        }

        // una vez terminado el recibo de datos
        // obtenemos el IP y puerto del emisor
        char addr_from[INET_ADDRSTRLEN];

        inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr),
                  addr_from, sizeof(addr_from));

        IP_from = addr_from;                                      // seteamos el ip
        Port_from = get_in_port((struct sockaddr *)&sender_addr); // seteamos el puerto

        return 0; // 0: success, -1: failure
    }


}

#endif //UDP_SOCKET_HPP_