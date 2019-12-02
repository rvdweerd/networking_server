#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib,"ws2_32.lib")

int main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr<<"Can't initialize winsock! Quittting." << std::endl;
		return 0;
	}
	
	// Create a socket (A type in windows. A file descriptor in Unix: a number that maps onto a file/device/etc.)
	// tcp socket (not dram)
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quittting." << std::endl;
		return 0;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000); // networking - big indian, pc - little indian. Host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // could also use inet_pton, bind it to the local lookup address
	bind(listening, (sockaddr*)&hint, sizeof(hint)); // bind this port on any address to the socket
		// now we can use the socket (bound to the networking codes) to reference the networking code to send/receive things using the socket

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN); // marks it as being for listening, doesn't set any cconnections. background SOMAXCONN
		// now one half of the two poll is bound

	// Wait for a connection
	sockaddr_in client; // create other socket address
	int clientsize = sizeof(client);
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);// connection happens on a port on the server. the socket that resturns clientsocket is some random number
	if (clientSocket == INVALID_SOCKET)
	{
	}
		// get some data  name, address, 
	char host[NI_MAXHOST]; // client's remote name
	char service[NI_MAXHOST]; // service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST) 
	ZeroMemory(service, NI_MAXHOST);
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // nodebuffer and nodebuffer size, service buffer and servicebuffer size
	{
		std::cout << host << " connected on port " << service << std::endl; // do dns lookup on client info
	}
	else // get ip address (in 4 bytes) as string of characters
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl; // network to host short, just ip address & port

	}

	// Close listening socket
	closesocket(listening);

	// while loop: accept and echo message back to client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		// wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);// receive function returns # bytes received. if 0, client disconnected
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in recv(). Quitting." << std::endl;
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
			break;
		}
		
		// echo message back to client
		send(clientSocket, buf, bytesReceived+1 , 0); // when we receive we don't get 0 terminate buffer. +1 automaticallly adds 0 
		if (bytesReceived > 0)
		{
			std::cout << "data echoed: " << buf << std::endl;
			if (buf[0] == 'q' && buf[1] == 'u' && buf[2] == 'i' && buf[3] == 't')
			{
				std::cout << "quitting." << std::endl;
				break;
			}
		}
	}

	// Close the socket
	closesocket(clientSocket);

	// Clean up winsock
	WSACleanup();
	
	std::cin.get();
	return 0;
}