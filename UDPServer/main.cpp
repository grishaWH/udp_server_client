#include "UDPServer.h"

int main() {
	UDPServer server(3397);
	server.start();
}