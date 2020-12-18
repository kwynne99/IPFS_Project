#pragma once
#include <WinSock2.h>
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <ctime>
#include <thread>
#include <string>
#include <vector>
#include <fstream>

#define BUFFER_SIZE 100

struct header {
	unsigned int seqnum;
	unsigned int acknum;
	unsigned int
		fin : 1,
		syn : 1,
		rst : 1,
		psh : 1,
		ack : 1,
		urg : 1;
	unsigned short int checksum;
	char payload[];
};

unsigned short int chksum(struct header* h) {
	short int sum = 0;
	sum += h->seqnum;
	sum += h->acknum;
	sum += h->fin;
	sum += h->syn;
	sum += h->rst;
	sum += h->psh;
	sum += h->ack;
	sum += h->urg;
	std::string data = h->payload;
	for (char const& c : data) {
		sum += (int)c;
	}
	return sum;
}

std::string appendZeroIfShort(std::string word, int length) {
	while (word.length() < length) {
		word = "0" + word;
	}
	return word;
}

std::string convertToEnd(std::string index) {
	std::string result(index.length(), '@');
	return result;
}

struct PARAMETERS {
	//std::string rName;
	std::string rPort;
};

struct PEER {
	char name;
	addrinfo address;
	std::string portNo;
	int freeSpace;
};

struct CLIENTLIST {
	std::vector<int> clientList;
};

class PeerList {
private:
	std::vector<PARAMETERS> peerL;
public:
	void addPeer(PARAMETERS);
	void removePeer(PARAMETERS);
	int peerListSize();
};

void PeerList::addPeer(PARAMETERS param) {
	peerL.push_back(param);
}

void PeerList::removePeer(PARAMETERS param) {
	for (int i = 0; i < peerL.size(); i++) {
		if (peerL[i].rPort == param.rPort) {
			peerL.erase(peerL.begin() + i);
		}
	}
}

int PeerList::peerListSize() {
	return peerL.size();
}

bool receiveHandShake() {
	int iResult;
	header sendbuff;
	header recvbuff;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof SenderAddr;
	SOCKET s = INVALID_SOCKET;
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	iResult = recvfrom(s, (char*)&recvbuff, BUFFER_SIZE, 0, (SOCKADDR*)&SenderAddr, (int*)&SenderAddrSize);
	if (recvbuff.syn == 1 && recvbuff.ack == 0) {
		sendbuff.acknum = recvbuff.seqnum + 1;
		sendbuff.seqnum = 0, sendbuff.syn = 1, sendbuff.ack = 1;
		iResult = sendto(s, (char*)&sendbuff, BUFFER_SIZE, 0, (SOCKADDR*)&SenderAddr, SenderAddrSize);
		iResult = recvfrom(s, (char*)&recvbuff, BUFFER_SIZE, 0, (SOCKADDR*)&SenderAddr, (int*)&SenderAddrSize);
		if (recvbuff.syn = 0, recvbuff.ack = 1, recvbuff.acknum = 1) {
			std::cout << "Connection established." << std::endl;
			return true;
		}
		else return false;
	}
	else return false;
}

bool sendHandShake(addrinfo send) {
	int iResult;
	header sendbuff;
	header recvbuff;
	SOCKET s = INVALID_SOCKET;
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sendbuff.seqnum = 0, sendbuff.syn = 1;
	iResult = sendto(s, (char*)&sendbuff, BUFFER_SIZE, 0, send.ai_addr, send.ai_addrlen);
	iResult = recvfrom(s, (char*)&recvbuff, BUFFER_SIZE, 0, 0, 0);
	if (recvbuff.syn == 1 and recvbuff.ack == 1 and recvbuff.acknum == 1) {
		sendbuff.acknum = 1, sendbuff.syn = 0, sendbuff.ack = 1);
		iResult = sendto(s, (char*)&sendbuff, BUFFER_SIZE, 0, send.ai_addr, send.ai_addrlen);
		return true;
	}
	else return false;
}