#include "NetReader.h"


NetReader::NetReader(std::string server, std::string port) : server(server), port(port)
{

#ifndef _WIN32
#error "Only works on Windows"
#endif

}

bool NetReader::start()
{

	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(server.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	// shutdown the connection since no more data will be sent
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return false;
	}

	readThread = std::thread(NetReader::readThreadFunction, this);

	return true;
}

//recv failed with error: 10038

NetReader::Orientation NetReader::getOrientation()
{
	return orientation;
}

NetReader::~NetReader()
{
	if(readThread.joinable()) 
		readThread.join();
}

void NetReader::readThreadFunction(NetReader* reader)
{
	int iResult = readData(reader);

	if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	// cleanup
	closesocket(reader->connectSocket);
	WSACleanup();

}

int NetReader::readData(NetReader* reader)
{
	int recvResult = synchronizeData(reader);
	if (recvResult <= 0) 
		return recvResult;

	char recvbuf[DEFAULT_BUFLEN];
	std::size_t read = 0;
	
	while (read < DEFAULT_BUFLEN) {
		int recvResult = recv(reader->connectSocket, recvbuf + read, DEFAULT_BUFLEN - read, 0);
		if (recvResult <= 0) 
			return recvResult;

		char* bufferEnd = recvbuf + read + recvResult;
		char* lastSeparator = findLastSeparator(recvbuf, bufferEnd);
		if (lastSeparator) {

			char* dataInit = findLastSeparator(recvbuf, lastSeparator);
			dataInit = dataInit ? dataInit + 1 : recvbuf;


			std::string data(dataInit, lastSeparator);

			if (data.empty())
				read = read;

			reader->processData(data);

			read = bufferEnd - lastSeparator - 1;
			memcpy(recvbuf, lastSeparator, read);

		}
		else {
			read += recvResult;
		}

	};
	return 0;

}

int NetReader::synchronizeData(NetReader* reader)
{
	char c;
	int recvResult = 0;
	do {
		recvResult = recv(reader->connectSocket, &c, 1, 0);
	} while (c != dataSeparator && recvResult > 0);
	return recvResult;
}

void NetReader::processData(std::string data)
{
	
	std::size_t sz, offset = 0;
	
	orientation.x = std::stof(data, &sz); offset += sz;
	orientation.y = std::stof(data.substr(offset + 1), &sz); offset += sz;
	orientation.z = std::stof(data.substr(offset + 1), &sz); offset += sz;

}

char * NetReader::findLastSeparator(char * begin, char * end)
{
	char* it = end;
	do {
		if (*--it == dataSeparator) return it;
	} while (it > begin);

	return nullptr;
}

