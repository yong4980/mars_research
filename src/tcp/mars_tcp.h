#ifndef _MARS_TCP_H_
#define _MARS_TCP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
//IPC
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <fcntl.h>
#include <semaphore.h>

#define PORT 5555
#define MAXCLIENT 30
#define MAXSOCKET MAXCLIENT+1
#define BUFFERSIZE 1023
#define SERVER 0
#define CLIENT 1

class Tcp{
private:
  struct sockaddr_in serverAddr; //Server Info
  struct sockaddr_in newClientAddr; //New Client Info
  socklen_t addr_size;
  char* buffer;
  int clientSocket, check;
  int* connectedSocket;
  char* hostIP;

  key_t key;
  int shmid;

public:
  Tcp();
  ~Tcp();
  int BuildServerTCP(); //make and bind socket
  int BuildClientTCP(char serverIP[]); //make socket and connect with server
  void ListeningClient(int newSocketNum); //waititng clients socket
  char* ReadMsg(int socketNum, int mode); //read msg
  void WriteMsg(int socketNum, char* msg, int mode); //send msg
  void WriteMsg(char* socketNumStr, char* msg); //Function overload
  int FindEmptySocket(); //find empty socekt and return that index
  void CheckConnectedSocket(); //show connected sockets
  void QuitTcp(); //release memory, etc..
  void QuitAll(); //terminate connected clients
  void close(int socketNum); //close socket
  void DisconnectSocket(int socketNum); //erase socket info
};

#endif
