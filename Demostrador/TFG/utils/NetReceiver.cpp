#include "NetReceiver.h"


NetReceiver::NetReceiver(std::string server, unsigned short port) : server(server), port(port)
{

#ifndef _WIN32
#error "Only works on Windows"
#endif

}

bool NetReceiver::start()
{

	WSADATA wsaData;
	
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
	
	// Create a SOCKET for connecting to server
	connectSocket = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {
		WSACleanup();
		return false;
	}

	sockaddr_in serverAddr;
	inet_pton(AF_INET, server.c_str(), &serverAddr.sin_addr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	// Connect to server.
	if (connect(connectSocket, (const sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
		WSACleanup();
		return false;
	}

	// shutdown the connection since no more data will be sent
	if (shutdown(connectSocket, SD_SEND) == SOCKET_ERROR) {
		closesocket(connectSocket);
		WSACleanup();
		return false;
	}

	reciveThread = std::thread(NetReceiver::recive_thread_function, this);

	return true;
}


NetReceiver::Orientation NetReceiver::get_orientation()
{
	return orientation;
}

NetReceiver::~NetReceiver()
{
	if(reciveThread.joinable()) 
		reciveThread.join();
}

void NetReceiver::recive_thread_function(NetReceiver* reader)
{
	int iResult = read_data(reader);

	if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	closesocket(reader->connectSocket);
	WSACleanup();

}

int NetReceiver::read_data(NetReceiver* reader)
{

	int recvResult = synchronize_data(reader);
	if (is_error_code(recvResult)) return recvResult;

	char recvbuf[DEFAULT_BUFLEN];
	std::size_t read = 0;
	
	while (read < DEFAULT_BUFLEN) {
		int recvResult = recv(reader->connectSocket, recvbuf + read, DEFAULT_BUFLEN - read, 0);
		if (recvResult <= 0) 
			return recvResult;

		char* bufferEnd = recvbuf + read + recvResult;
		char* lastSeparator = find_last_separator(recvbuf, bufferEnd);

		if (lastSeparator) {

			char* dataInit = find_last_separator(recvbuf, lastSeparator);
			dataInit = dataInit ? dataInit + 1 : recvbuf;


			std::string data(dataInit, lastSeparator);
			if (!data.empty()) reader->process_data(data);

			read = bufferEnd - lastSeparator - 1;
			memcpy(recvbuf, lastSeparator, read);


		}
		else {
			read += recvResult;
		}

	};
	return 0;

}

int NetReceiver::synchronize_data(NetReceiver* reader)
{
	char c;
	int recvResult = 0;
	do {
		recvResult = recv(reader->connectSocket, &c, 1, 0);
	} while (c != dataSeparator && recvResult > 0);
	return recvResult;
}

void NetReceiver::process_data(std::string data)
{
	
	std::size_t sz, offset = 0;
	
	orientation.x = std::stof(data, &sz); offset += sz;
	orientation.y = std::stof(data.substr(offset + 1), &sz); offset += sz;
	orientation.z = std::stof(data.substr(offset + 1), &sz); offset += sz;

}

char * NetReceiver::find_last_separator(char * begin, char * end)
{
	char* it = end;
	do {
		if (*--it == dataSeparator) return it;
	} while (it > begin);

	return nullptr;
}

bool NetReceiver::is_error_code(int code)
{
	return code <= 0;
}

