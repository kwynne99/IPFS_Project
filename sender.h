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
#include "receiver.h"
#include "fs.h"
#define MAX_COMMAND_LEN 80
#define MAX_BUFFER_LEN 512
#define MAX_PACKET_CHUNK_LEN 1024
#define BUFFER_SIZE 256
#define HOSTNAME "127.0.0.1"
#pragma comment(lib, "Ws2_32.lib")


class Sender {
private:
	//std::string senderName = "";
	std::string senderPort;
	std::vector<addrinfo> clientList;
	int clientCount;
	std::vector<PEER> peerList;
public:
	Sender();
	void registerPeer();
	void connectPeer();
	void connectSelf(std::string senderPort);
	void cmdPrompt();
};
Sender::Sender() {
	clientList.reserve(5);
	clientCount = 0;
}

void Sender::registerPeer() {
	//std::string hostname;
	std::string portno;
	//std::cout << "Enter peer hostname to connect: " << std::endl;
	//std::cin >> hostname;
	std::cout << "Enter peer port number to add: " << std::endl;
	std::cin >> portno;
	std::cin.get();
	//this->senderName = hostname;
	this->senderPort = portno;
	connectPeer();
}

void Sender::connectSelf(std::string portNo) {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	iResult = getaddrinfo(HOSTNAME, portNo.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed sender: %d\n", iResult);
		WSACleanup();
		exit(0);
	}
	ptr = result;
	if (clientCount == 0) {
		clientList.push_back(*ptr);
		clientCount++;
	}
	else {
		for (int i = 0; i < clientList.size(); i++) {
			if (clientList[i].ai_addr == ptr->ai_addr) {
				std::cout << "Peer already on list." << std::endl;
				break;
			}
		}
		clientList.push_back(*ptr);
		clientCount++;
	}
	PEER peer;
	peer.portNo = portNo;
	peer.address = *ptr;
	peer.name = '0' + (atoi(portNo.c_str()) % 8000);
	peerList.push_back(peer);
}

void Sender::connectPeer() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	iResult = getaddrinfo(HOSTNAME, senderPort.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed sender: %d\n", iResult);
		WSACleanup();
		exit(0);
	}
	ptr = result;
	if (clientCount == 0) {
		clientList.push_back(*ptr);
		clientCount++;
	}
	else {
		for (int i = 0; i < clientList.size(); i++) {
			if (clientList[i].ai_addr == ptr->ai_addr) {
				std::cout << "Peer already on list." << std::endl;
				break;
			}
		}
		clientList.push_back(*ptr);
		clientCount++;
	}
	PEER peer;
	peer.portNo = senderPort;
	peer.address = *ptr;
	peer.name = '0' + (atoi(senderPort.c_str()) % 8000);
	peerList.push_back(peer);
	/*SOCKET SendSocket = INVALID_SOCKET;
	ptr = result;
	SendSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (SendSocket == INVALID_SOCKET) {
		printf("Error at socket: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(0);
	}
	*/
	freeaddrinfo(result);
	cmdPrompt();
/*	printf("Command List:\n");
	printf("Type 'exit' to exit program.\n");
	printf("Type 'connect' to connect to another peer.\n");
	printf("Type anything else to send a message to all peers.\n");
	while (strcmp(buffer, "exit")) {
		if (!strcmp(buffer, "connect")) {
			registerPeer();
		}
		//printf("Send a test message to peer...\n");
		std::cin.sync();
		std::cin.getline(buffer, 256);
		//for (SOCKET client : clientList) {
		iResult = sendto(SendSocket, buffer, 256, 0, ptr->ai_addr, (int)ptr->ai_addrlen);
		//}
		//std::cout << "Press a to send another message." << std::endl;
		//std::cin.sync();
		//std::cin.getline(buffer, 256);
	}
	printf("Done sending messages.\n");
	iResult = shutdown(SendSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("Shutdown failed: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		WSACleanup();
		exit(0);
	}
*/
}

void Sender::cmdPrompt() {
	discEmulator sandisc;
	int iResult;
	char buffer[256];
	SOCKET SendSocket = socket(AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);
	printf("Command List:\n");
	printf("Type 'exit' to exit program.\n");
	printf("Type 'connect' to connect to another peer.\n");
	printf("Type anything else to send a message to all peers.\n");
	while (TRUE) {
		std::cin.sync();
		std::cin.getline(buffer, 256);
		std::string temp = buffer;
		if (!strcmp(buffer, "connect")) {
			registerPeer();
		}
		else if (!strcmp(buffer, "list")) {
			std::cout << "Peer Address List:" << std::endl;
			for (PEER &peer : peerList) {
				std::cout << "Peer " << peer.name << "\t Address: " << peer.address.ai_addr << "\t PortNo: " << peer.portNo << std::endl;
			}
		}
		else if (!temp.compare(0, 5, "touch")) {
			std::string fName, fText;
			/*for (addrinfo& client : clientList) {
				iResult = sendto(SendSocket, buffer, 256, 0, client.ai_addr, (int)client.ai_addrlen);
			}*/
			//auto first_token = s.substr(0, s.find(' '));
			//std::string command = "touch file1 refrigerator, oven, microwave, cabinets.";
			std::string command = buffer;
			std::string operation;
			std::string fileName;
			std::string fileText;
			int index = 0;
			while (command[index] != ' ') {
				operation += command[index];
				index++;
			}
			index++;
			while (command[index] != ' ') {
				fileName += command[index];
				index++;
			}
			index++;
			while (command[index] != '\0') {
				fileText += command[index];
				index++;
			}
			std::cout << operation << " " << fileName << " " << fileText << std::endl;
			//fName = temp.substr(6, 4); // touch test hello
			//fText = temp.substr(11, 5);		//01234567891011
			fName = fileName;
			fText = fileText;
			printf("Run touch and send blocks.\n");
			//After sending block, wait for response from that peer with location.
			sandisc.touch(fName, fText, peerList);
			Sleep(1000);
			printf("Touch command completed.\n");
		}
		else if (!strcmp(buffer, "exit")) {
			for (addrinfo &client : clientList) {
				iResult = sendto(SendSocket, buffer, 256, 0, client.ai_addr, (int)client.ai_addrlen);
			}
			printf("Done sending messages.\n");
			iResult = shutdown(SendSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				printf("Shutdown failed: %d\n", WSAGetLastError());
				closesocket(SendSocket);
				WSACleanup();
				exit(0);
			}
			closesocket(SendSocket);
			WSACleanup();
			break;
		}
		//printf("Send a test message to peer...\n");
		else {
			for (addrinfo &client : clientList) {
				iResult = sendto(SendSocket, buffer, 256, 0, client.ai_addr, (int)client.ai_addrlen);
			}
		}
		//std::cout << "Press a to send another message." << std::endl;
		//std::cin.sync();
		//std::cin.getline(buffer, 256);
	}
/*	printf("Done sending messages.\n");
	iResult = shutdown(SendSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("Shutdown failed: %d\n", WSAGetLastError());
		closesocket(SendSocket);
		WSACleanup();
		exit(0);
	}
	closesocket(SendSocket);
	WSACleanup();*/
}
