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
	void printPeerList();
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

/*void PeerList::printPeerList() {
	int count = 1;
	for (auto& it : peerL) {
		std::cout << count << ": " << it.rName << ":" << it.rPort << '\n';
		count++;
	}
}*/