#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <WS2tcpip.h>
#include <ctime>
#include <thread>
#include <string>
#include <process.h>
#include "receiver.h"
#include "sender.h"
#include "helper.h"
#pragma comment(lib, "Ws2_32.lib")
#define PORT "8080"
using namespace std;

//void startReceiver(string rName, string rPort) {
//	unique_ptr<Receiver> rec(new Receiver(rName, rPort));
//	rec->startReceiver();
//}

//void startSender() {
//	Sleep(5000);
//	unique_ptr<Sender> send(new Sender());
//	//send->registerPeer();
//	send->connectPeer();
//}

DWORD WINAPI ReceiverThread(LPVOID lpParameter) {
	PARAMETERS* params = (PARAMETERS*)lpParameter;
	//unique_ptr<Receiver> rec(new Receiver(params->rName, params->rPort));
	unique_ptr<Receiver> rec(new Receiver(params->rPort));
	rec->startReceiver();
	return 0;
}

DWORD WINAPI SenderThread(LPVOID lpParameter) {
	PARAMETERS* params = (PARAMETERS*)lpParameter;
	Sleep(1000);
	unique_ptr<Sender> send(new Sender());
	send->connectSelf(params->rPort);
	send->registerPeer();
	//send->connectPeer();
	return 0;
}

int main() {
	PARAMETERS params;
	//string rName = "";
	string rPort = "";
	//cout << "Enter the hostname:\n";
	//cin >> rName;
	cout << "Enter port number to start peer on:\n";
	cin >> rPort;
	/*thread Receiver(startReceiver, rName, rPort);
	thread Sender(startSender);
	*/
	//params.rName = rName;
	params.rPort = rPort;
	DWORD dwThreadId;
	HANDLE ReceiverThd, SenderThd;
	ReceiverThd = CreateThread(NULL, 0, ReceiverThread, &params, 0, &dwThreadId);
	SenderThd = CreateThread(NULL, 0, SenderThread, &params, 0, &dwThreadId);
	WaitForSingleObject(ReceiverThd, INFINITE);
	WaitForSingleObject(SenderThd, INFINITE);
}


// Sender needs to send to all peers, including itself. The receiver thread will then handle all FAT and allocation.
// When port is set that needs to be sent to sender thread in addition to the receiver thread.
