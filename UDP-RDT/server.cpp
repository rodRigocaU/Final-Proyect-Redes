// Server side implementation of UDP client-server model
#include "UDP_RDT_SOCKET.hpp"
// #include "UDP_SOCKET.hpp"
// #include "SOCKET.hpp"

using namespace std;

// Driver code
int main() {
	std::string myPort = "8080";

	RDT::UdpSocket server_socket("", myPort);

	std::string IP_from, message;
	uint16_t Port_from;

	u_char buffer[MAX_DGRAM_SIZE];
	
	while(true){
		// server_socket.simpleRecv(buffer, IP_from, Port_from); // leemos los datos y los guardamos en el string message
	 	server_socket.receive(message, IP_from, Port_from);
		 cout << "mensaje desde: " << IP_from << "  puerto: " << Port_from << endl << endl;
		cout << " mensaje: " << message << endl;
	}
	return 0;
}



