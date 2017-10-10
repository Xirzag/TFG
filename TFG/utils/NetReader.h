#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <thread>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


class NetReader
{
public:
	NetReader(std::string server, std::string port);
	bool start();

	struct Orientation {
		float x, y, z;
	};

	Orientation getOrientation();

	~NetReader();

private:
	std::string server;
	std::string port;
	std::thread readThread;
	SOCKET connectSocket = INVALID_SOCKET;;
	Orientation orientation;

	const static std::size_t DEFAULT_BUFLEN = 1000;
	const static char dataSeparator = '\n';
	void static readThreadFunction(NetReader* reader);
	int static readData(NetReader* reader);
	int static synchronizeData(NetReader* reader);
	void processData(std::string data);
	char static *findLastSeparator(char* begin, char* end);

};

#undef WIN32_LEAN_AND_MEAN