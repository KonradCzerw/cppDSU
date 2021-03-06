#include "udp.hpp"

#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>

WSASession::WSASession() {
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
}

WSASession::~WSASession() {
	WSACleanup();
}


UDPAddress::UDPAddress(const std::string& address, unsigned short port) {
	this->addr.sin_family = AF_INET;
	inet_pton(AF_INET, address.c_str(), &this->addr.sin_addr);
	this->addr.sin_port = htons(port);
}


UDPSocket::UDPSocket() {
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
}

UDPSocket::~UDPSocket() {
	closesocket(sock);
}

void UDPSocket::SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags) {
	sockaddr_in add;
	add.sin_family = AF_INET;
	inet_pton(AF_INET, address.c_str(), &add.sin_addr);
	add.sin_port = htons(port);
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}

void UDPSocket::SendTo(UDPAddress* address, const char* buffer, int len, int flags) {
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address->addr), sizeof(address->addr));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}

void UDPSocket::SendTo(sockaddr_in& address, const char* buffer, int len, int flags) {
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}

sockaddr_in UDPSocket::RecvFrom(char* buffer, int len, int flags) {
	sockaddr_in from;
	int size = sizeof(from);
	int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

	// make the buffer zero terminated
	buffer[ret] = 0;
	return from;
}
void UDPSocket::Bind(unsigned short port) {
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons(port);

	int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
}
