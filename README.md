# NFS_p2p
Network File System over P2P Network

Created by: Kyle Wynne, Anton Marku, Brandon Lao, and Enis Tahiraj.

Network: Kyle Wynne
File System: Anton Marku
Blockchain: Brandon Lao & Enis Tahiraj

Our program uses a Peer-to-peer network to distribute files to peers through the network.
If a peer creates a file, it will be separated into blocks and then distributed to peers on the network, prioritizing
peers with the most available free space first. This allows for equal load distribution. Once the peer node receives
the block it will then allocate it to its storage and send back where it allocated it it's file system.
Finally once all blocks have been allocated, an the new FAT will be sent out to all peer nodes and updated.
The FAT contains the filename, and which peer has which block.
During this process the blockchain keeps track of transactions taken by nodes on their storage and fat.

WIP:
1) Smoother implementation of blockchain for tracking transactions.
2) Additional features and commands to better simulate a functioning file system.
