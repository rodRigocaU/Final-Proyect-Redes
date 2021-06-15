// Server side implementation of UDP client-server model
#include "UDP_RDT_SOCKET.hpp"
// #include "UDP_SOCKET.hpp"

using namespace std;

// Driver code
int main() {
	std::string myPort = "8080";

	RDT::UdpSocket server_socket("", myPort);
	std::string message, IP_from;
	uint16_t Port_from;
	while(true){
		//server_socket.receive();
		server_socket.receive(message, IP_from, Port_from); // leemos los datos y los guardamos en el string message
	 	cout << "tamaño del mensaje: " << message.size() << "  desde: " << IP_from << "  puerto: " << Port_from << endl << endl;
		cout << "mensaje: " << message << endl;
		
	}
	return 0;
}



