#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <ctime>
#include <thread>
#include <string>
#include <vector>
#include "helper.h"
#include "sender.h"
#include "fs.h"
#pragma comment(lib, "Ws2_32.lib")
#define BUFFER_SIZE 256
#define HOSTNAME "127.0.0.1"


class Receiver {
private:
	//std::string receiverName = "";
	std::string receiverPort = "";
	int sockfd = 0, newsockfd = 0, portno = 0;
	socklen_t clientLen;
	struct sockaddr_in serv_addr, client_addr;
	//char buffer[256];
	struct CLIENTLIST clientList;
	std::vector<int> peerList;
public:
	//Receiver(std::string, std::string);
	Receiver(std::string);
	void startReceiver();
};

/*Receiver::Receiver(std::string rName, std::string rPort) {
	//this->receiverName = rName;
	this->receiverPort = rPort;
}*/

Receiver::Receiver(std::string rPort) {
	this->receiverPort = rPort;
}

void Receiver::startReceiver() {
	// Create FAT
	char peername;
	std::string fileName;
	peername = '0' + (atoi(receiverPort.c_str()) % 8000);
	fileName = peername;
	fileName += "disc.txt";
	discEmulator sandisc(fileName);
	//discEmulator sandisc = discEmulator();
	WSADATA wsaData;
	int iResult;
	int totalReceived = 0, totalSent = 0;
	//int i;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
		exit(0);
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_DGRAM; // Stream socket (TCP)
	hints.ai_protocol = IPPROTO_UDP; // Specifies UDP protocol
	hints.ai_flags = AI_PASSIVE; // Return socket address structure in call to bind

	iResult = getaddrinfo(HOSTNAME, receiverPort.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed receiver: %d\n", iResult);
		WSACleanup();
		exit(0);
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(), %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(0);
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("Bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}
//	freeaddrinfo(result); // No longer needed

	printf("** Receiver started **\n");
	printf("Hostname: %s\nPort#:%s\n", HOSTNAME, receiverPort.c_str());

	//SOCKET ClientSocket; // Temporary socket used to accept connections.
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof SenderAddr;
	while (TRUE) {
		char buffer[256];
		iResult = recvfrom(ListenSocket, buffer, BUFFER_SIZE, 0, (SOCKADDR *)&SenderAddr, (int*)&SenderAddrSize);
		if (iResult > 0) {
			if (!strcmp(buffer, "exit")) {
				std::cout << SenderAddr.sin_port << " has shutdown." << std::endl; // Remove this node from table.
			}
			//else if (!temp.compare(0, 5, "touch")) {
			else if (!strcmp(buffer, "touch")) {
				printf("Receiver: touch command has been used, waiting to receive block...\n");
				// Get ready for incoming block and then allocate.
				char blockBuffer[20];
				iResult = recvfrom(ListenSocket, blockBuffer, 20, 0, (SOCKADDR*)&SenderAddr, (int*)&SenderAddrSize);
				// After allocating block, send location back.
				std::string allocString = blockBuffer;
				std::cout << "Receiver: " << allocString << std::endl;
				int freeBlock = sandisc.getTopFreeBlockIndex();
				sandisc.allocate(allocString, sandisc.getTopFreeBlockIndex());
				printf("Receiver: Successfully allocated block.\nSending block location to update FAT.\n");
				sendto(ListenSocket, (char*)&freeBlock, sizeof freeBlock, 0, (SOCKADDR*)&SenderAddr, SenderAddrSize);				
			}
			else if (!strcmp(buffer, "update")) {
				// Update fat
				char fatBuffer[256];
				recvfrom(ListenSocket, fatBuffer, BUFFER_SIZE, 0, (SOCKADDR*)&SenderAddr, (int*)&SenderAddrSize);
				std::string newFat = fatBuffer;
				std::cout << "New FAT entry: " << newFat << std::endl;
				sandisc.updateFat(newFat);
				printf("Receiver: FAT updated.\n");
			}
			else if (!strcmp(buffer, "freeSpace")) {
				int freeSpace = sandisc.getFreeSpace();
				sendto(ListenSocket, (char*)&freeSpace, sizeof freeSpace, 0, (SOCKADDR*)&SenderAddr, SenderAddrSize);
			}
			else {
				std::cout << SenderAddr.sin_port << ": " << buffer << std::endl;
			}
		}
	} //while (iResult > 0);
}
