#include "Network/UdpSocket.hpp"

namespace net{

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

    if (IP.empty())
      freeaddrinfo(servinfo);
  }

  UdpSocket::~UdpSocket()
  {
    // liberamos la memoria de la lista de resultados
    freeaddrinfo(servinfo);

    // cerramos nuestro sockets
    close(socket_file_descriptor);
  }

  int UdpSocket::getSocketFileDescriptor() const 
  {
    return socket_file_descriptor;
  }

  std::string UdpSocket::getIP() const
  {
    return IP;
  }

  uint16_t UdpSocket::getPort() const
  {
    return std::stoi(Port);
  }

  std::string UdpSocket::getSenderIP()
  {
    char addr_from[INET_ADDRSTRLEN];

    inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr),
                    addr_from, sizeof(addr_from));
    
    return std::string(addr_from);
  }

  uint16_t UdpSocket::getSenderPort()
  {
    return get_in_port((struct sockaddr *)&sender_addr);
  }

  int UdpSocket::sendAll(u_char *buffer, int &bytes_sent, bool to_sender)
  {
    int total = 0;                   // cuantos bytes hemos enviado
    int bytes_left = MAX_DGRAM_SIZE; // cuantos nos falta por enviar
    int n;
    while (total < MAX_DGRAM_SIZE)
    {
      // si el paquete es para el host que nos envió un mensaje
      // o si es para el socket que tenemos configurado
      if (to_sender)
        n = sendto(socket_file_descriptor, buffer + total, bytes_left, 0,
                  (struct sockaddr *)&sender_addr, sizeof(struct sockaddr_storage));
      else
        n = sendto(socket_file_descriptor, buffer + total, bytes_left, 0,
                  configured_sockaddr->ai_addr, configured_sockaddr->ai_addrlen);

      if (n == -1)
        break;
      total += n;
      bytes_left -= n;
    }
    bytes_sent = total;      // la cantidad de bytes enviados realmente
    return n == -1 ? -1 : 0; // return -1 cuano falla, 0 cuando es exitoso
  }

  int UdpSocket::simpleRecv(u_char* buffer)
  {
    int n;
    n = recvfrom(socket_file_descriptor, buffer, MAX_DGRAM_SIZE, 0,
                  (struct sockaddr *)&sender_addr, &sender_addr_len);

    // 0 cuando el host externo cerró la conexión
    if (n == 0)
      std::cerr << "El ordenador de destino se desconectó inesperadamente...\n";

    return n; // MAX_DGRAM_SIZE cuando hay éxito, -1 en el otro caso
  }

  void* UdpSocket::get_in_addr(struct sockaddr *sa)
  {
    if (sa->sa_family == AF_INET)
      return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }

  uint16_t UdpSocket::get_in_port(struct sockaddr *sa)
  {
    if (sa->sa_family == AF_INET)
      return ((struct sockaddr_in *)sa)->sin_port;
    return ((struct sockaddr_in6 *)sa)->sin6_port;
  }

}