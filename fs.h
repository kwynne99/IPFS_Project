#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "helper.h"
#include "sender.h"

using namespace std;

class discEmulator {
private:
    //these can only be altered by touch, mkdir, append, write
    string disc;
    int topFreeBlockIndex = 0;
    //freeBlocksQuantity is based on blockQuantity and can be altered by the constructor
    //  that has parameters but is also reduced/increase when blocks are allocated/deallocated
    int freeBlockQuantity = 50;
    //specifications that can be altered by the constructor that has parameters
    int blockQuantity = 50;
    int blockSize = 20;
    int fatSize = 300;
    int lineLength = 100;//length of lines in disc.txt
    string fileName;
    int indexLength = 2;//number of characters in blocks reserved for indexes
public:
    //constructor that uses default specifications
    discEmulator();
    discEmulator(string fN);
    //constructor that uses user's specifications
    //  blockSize must be >= 1 + 2 * indexLength
    //  if it is not, blockSize will be increased until it is
    //todo: save user specifications so they can be accessed after
    //  program stops running
    discEmulator(int bQ, int bS, int fS, int lL, string fN, int iL);
    //writes the disc string to disc.txt
    void paste();
    //writes text to a file with the name as fileName
    void touch(string fileName, string text, vector<PEER> peerList);
    //allocates text at the top free space, decrements topFreeBlockIndex and freeBlockQuantity
    void allocate(string text, int indexToInsert);
    //given a block index, returns index of linked block
    int getNextBlockIndex(int index);
    //changes corresponding private variable
    void setTopFreeBlockIndex(int num);
    void setFreeBlockQuantity(int num);
    //inserts newFatEntry into fat
    void updateFat(string newFatEntry);
    //return index of top free block
    int getTopFreeBlockIndex();
    int getFreeSpace();
};

/*int main() {
    discEmulator sandisc = discEmulator();
    sandisc.paste();
    sandisc.touch("advice", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    sandisc.touch("query", "how old is the oldest person who ever lived?");
    cout << sandisc.getTopFreeBlockIndex() << endl;
    cout << sandisc.getNextBlockIndex(20) << endl;
}*/

//todo: save user specifications to disc.txt so they can be reused after program stops running
//  user can still technically just run the constructor with parameters again with the same
//  inputs and that would acheive the same thing
discEmulator::discEmulator(int bQ, int bS, int fS, int lL, string fN, int iL) {
    freeBlockQuantity = bQ;
    blockQuantity = bQ;
    blockSize = bS;
    fatSize = fS;
    lineLength = lL;
    fileName = fN;
    indexLength = iL;
    if (blockSize < (1 + 2 * indexLength)) {
        blockSize = 1 + 2 * indexLength;
    }
    string temp_str;
    ifstream read(fileName);
    while (getline(read, temp_str)) {
        disc += temp_str;
    }
    read.close();
    if (disc == "") {
        for (int i = 0; i < fatSize; i++) {
            disc += '@';
        }
        for (int i = 0; i < blockQuantity; i++) {
            string block(blockSize - indexLength, '@');
            string nextBlockIndex = to_string(i + 1);
            nextBlockIndex = appendZeroIfShort(nextBlockIndex, indexLength);
            if (i == blockQuantity - 1) {
                nextBlockIndex = convertToEnd(nextBlockIndex);
            }
            block += nextBlockIndex;
            disc += block;
        }
        disc += appendZeroIfShort(to_string(0), indexLength);
        disc += appendZeroIfShort(to_string(blockQuantity), indexLength);
    }
    else {
        topFreeBlockIndex = stoi(disc.substr(fatSize + blockQuantity * blockSize, indexLength));
        freeBlockQuantity = stoi(disc.substr(fatSize + blockQuantity * blockSize + indexLength, indexLength));
    }
}

discEmulator::discEmulator() {
    fileName = "disc.txt";
    string temp_str;
    ifstream read(fileName);
    while (getline(read, temp_str)) {
        disc += temp_str;
    }
    read.close();
    if (disc == "") {
        for (int i = 0; i < fatSize; i++) {
            disc += '@';
        }
        for (int i = 0; i < blockQuantity; i++) {
            string block(blockSize - 2, '@');
            string nextBlockIndex = to_string(i + 1);
            nextBlockIndex = appendZeroIfShort(nextBlockIndex, indexLength);
            if (i == blockQuantity - 1) {
                nextBlockIndex = convertToEnd(nextBlockIndex);
            }
            block += nextBlockIndex;
            disc += block;
        }
        disc += appendZeroIfShort(to_string(0), indexLength);
        disc += appendZeroIfShort(to_string(blockQuantity), indexLength);
    }
    else {
        topFreeBlockIndex = stoi(disc.substr(fatSize + blockQuantity * blockSize, indexLength));
        freeBlockQuantity = stoi(disc.substr(fatSize + blockQuantity * blockSize + indexLength, indexLength));
        //cout<<topFreeBlockIndex<<" "<<freeBlockQuantity<<endl;
    }
}

discEmulator::discEmulator(string fN) {
    fileName = fN;
    string temp_str;
    ifstream read(fileName);
    while (getline(read, temp_str)) {
        disc += temp_str;
    }
    read.close();
    if (disc == "") {
        for (int i = 0; i < fatSize; i++) {
            disc += '@';
        }
        for (int i = 0; i < blockQuantity; i++) {
            string block(blockSize - 2, '@');
            string nextBlockIndex = to_string(i + 1);
            nextBlockIndex = appendZeroIfShort(nextBlockIndex, indexLength);
            if (i == blockQuantity - 1) {
                nextBlockIndex = convertToEnd(nextBlockIndex);
            }
            block += nextBlockIndex;
            disc += block;
        }
        disc += appendZeroIfShort(to_string(0), indexLength);
        disc += appendZeroIfShort(to_string(blockQuantity), indexLength);
    }
    else {
        topFreeBlockIndex = stoi(disc.substr(fatSize + blockQuantity * blockSize, indexLength));
        freeBlockQuantity = stoi(disc.substr(fatSize + blockQuantity * blockSize + indexLength, indexLength));
        //cout<<topFreeBlockIndex<<" "<<freeBlockQuantity<<endl;
    }
}

void discEmulator::paste() {
    ofstream write(fileName);
    for (int i = 0; i < disc.size(); i += lineLength) {
        write << disc.substr(i, lineLength) << endl;
    }
    write.close();
}

void discEmulator::setTopFreeBlockIndex(int num) {
    string newTop = appendZeroIfShort(to_string(num), indexLength);
    int startIndex = fatSize + blockQuantity * blockSize;
    for (int i = startIndex; i < startIndex + indexLength; i++) {
        disc[i] = newTop[i - startIndex];
    }
    paste();
}

void discEmulator::setFreeBlockQuantity(int num) {
    string newQuantity = appendZeroIfShort(to_string(num), indexLength);
    int startIndex = fatSize + blockQuantity * blockSize + indexLength;
    for (int i = startIndex; i < startIndex + indexLength; i++) {
        disc[i] = newQuantity[i - startIndex];
    }
    paste();
}

int discEmulator::getNextBlockIndex(int index) {
    string indexString = disc.substr(fatSize + index * blockSize + (blockSize - indexLength), indexLength);
    if (indexString == "@@") {
        return -1;
    }
    else {
        return stoi(disc.substr(fatSize + index * blockSize + (blockSize - indexLength), indexLength));
    }
}

void discEmulator::allocate(string text, int indexToInsert) {
    cout << text << endl;
    int count{ 0 };
    int startIndex = fatSize + indexToInsert * blockSize;
    for (int i = startIndex; i < startIndex + blockSize; i++) {
        disc[i] = text[count];
        count++;
        if (count == text.length())
            break;
    }
    setTopFreeBlockIndex(indexToInsert + 1);
    topFreeBlockIndex = indexToInsert + 1;
    freeBlockQuantity--;
    setFreeBlockQuantity(freeBlockQuantity);
    //paste();
}
// To do: Use free space management to choose peers to allocate blocks to.
// Idea: Request free space available from each peer, then allocate to peers with most free space first.
void discEmulator::touch(string fileName, string text, vector<PEER> peerList) {
    //send signal for users to be in receive mode
    printf("Touch has been called.\n");
    SOCKET SendSocket = socket(AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP);
    string newFatEntry = fileName;
    int recvTopFreeBlock;
    discEmulator recv;
    // Free space management:
    std::multimap<int, PEER, std::greater<int>> peerMap;
    for (PEER& peer : peerList) {
        char buffer[256] = "freeSpace";
        sendto(SendSocket, buffer, 256, 0, peer.address.ai_addr, peer.address.ai_addrlen);
        recvfrom(SendSocket, (char*)&peer.freeSpace, sizeof peer.freeSpace, 0, 0, 0);
        std::cout << "Peer " << peer.name << "\tFree Space: " << peer.freeSpace << std::endl;
        //std::multimap<int, PEER, std::greater<int>> peerMap;
        peerMap.insert({ peer.freeSpace, peer });
    }
    std::cout << "Peers in order of free space." << std::endl;
    for (auto const& pair : peerMap) {
        std::cout << "Peer " << pair.second.name << "\tFree Space: " << pair.first << std::endl;
    }
    // Now I have the ordered multimap using freeSpace as the key.
    // Idea: create vector of strings to create blocks to be allocated. Then go through that vector sending blocks to peers in order until all blocks are sent.
    for (int i = 0; i < text.length(); i += blockSize) {
        string newBlock = text.substr(i, blockSize);
        char buffer[256] = "touch";
        sendto(SendSocket, buffer, 256, 0, peerList[i / 20 % peerList.size()].address.ai_addr, peerList[i / 20 % peerList.size()].address.ai_addrlen);
        sendto(SendSocket, newBlock.c_str(), blockSize, 0, peerList[i / 20 % peerList.size()].address.ai_addr, peerList[i / 20 % peerList.size()].address.ai_addrlen);
        recvfrom(SendSocket, (char*)&recvTopFreeBlock, sizeof recvTopFreeBlock, 0, 0, 0);
        printf("Sender: Received BlockIndex for FAT entry: %d\n", recvTopFreeBlock);
        newFatEntry += 'p';
        newFatEntry += peerList[i / 20 % peerList.size()].name;
        newFatEntry +=',';
        newFatEntry += to_string(recvTopFreeBlock) + '/';
    }
    for (PEER& peer : peerList) {
        char buffer[256] = "update";
        sendto(SendSocket, buffer, 256, 0, peer.address.ai_addr, peer.address.ai_addrlen); // Signals peers to receive a newFatEntry to update their FAT.
        sendto(SendSocket, newFatEntry.c_str(), 256, 0, peer.address.ai_addr, peer.address.ai_addrlen); // Sending the newFatEntry
    }
    peerMap.clear();
    /*for (int i = 0; i < text.length(); i += blockSize - indexLength) {
        newFatEntry += to_string(topFreeBlockIndex) + "/";
        string newBlock = text.substr(i, blockSize - indexLength);
        int indexToInsert = topFreeBlockIndex;
        int nextBlock = getNextBlockIndex(indexToInsert);
        topFreeBlockIndex = nextBlock;
        setTopFreeBlockIndex(topFreeBlockIndex);
        freeBlockQuantity--;
        setFreeBlockQuantity(freeBlockQuantity);
        while (newBlock.length() < blockSize - indexLength) {
            newBlock += '@';
        }
        if (!(i + blockSize - indexLength >= text.length())) {
            newBlock += appendZeroIfShort(to_string(nextBlock), indexLength);
        }
        else {
            string lastMarker(indexLength, '@');
            newBlock += lastMarker;
            cout << lastMarker << endl;
        }
        allocate(newBlock, indexToInsert);
    }

    int entryLength = newFatEntry.length();
    int entryIndex = 0;
    for (int i = 0; i < fatSize; i++) {
        if (disc[i] == '@' && entryLength > 0) {
            entryLength--;
            disc[i] = newFatEntry[entryIndex];
            entryIndex++;
        }
    }
    paste();
    */
}

void discEmulator::updateFat(string newFatEntry) {
    int entryLength = newFatEntry.length();
    int entryIndex = 0;
    for (int i = 0; i < fatSize; i++) {
        if (disc[i] == '@' && entryLength > 0) {
            entryLength--;
            disc[i] = newFatEntry[entryIndex];
            entryIndex++;
        }
    }
    paste();
}

int discEmulator::getTopFreeBlockIndex() {
    return stoi(disc.substr(fatSize + blockQuantity * blockSize, indexLength));
}

int discEmulator::getFreeSpace() {
    return freeBlockQuantity;
}

/*
nested loop to loop through table of peers
--users are represented by characters
fat and blocks needs to write the user as well as block number
  <this should come from the loop of the peers table and shorten text length from 18 to 17.
done: need to have fat updater function that receives new fat
done: touch needs to determine next free block instead of allocate
done: function to report top free block
*/