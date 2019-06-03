#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

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

//! A Tcp client that recives the orientation transmitted by the SensorTransmitter app.
class NetReceiver
{
public:
	NetReceiver(std::string server, unsigned short port);
	//! Start the receiver
	bool start();
	
	//! Orientation specified in euler angles
	struct Orientation {
		float x, y, z;
	};

	//! Return the orientation transmitted by the device
	Orientation get_orientation();

	~NetReceiver();

private:
	std::string server;
	unsigned short port;
	std::thread reciveThread;
	SOCKET connectSocket = INVALID_SOCKET;
	Orientation orientation;

	const static std::size_t DEFAULT_BUFLEN = 1000;
	const static char dataSeparator = '\n';
	void static recive_thread_function(NetReceiver* reader);
	int static read_data(NetReceiver* reader);
	int static synchronize_data(NetReceiver* reader);
	void process_data(std::string data);
	char static *find_last_separator(char* begin, char* end);
	bool static is_error_code(int code);

};

#undef WIN32_LEAN_AND_MEAN