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
  int BuildServerTCP();
  int BuildClientTCP(char serverIP[]);
  void ListeningClient(int newSocketNum);
  char* ReadMsg(int socketNum, int mode);
  void WriteMsg(int socketNum, char* msg, int mode);
  void WriteMsg(char* socketNumStr, char* msg); //Function overload
  int FindEmptySocket();
  void CheckConnectedSocket();
  int GetMaxClientNum();
  void QuitTCP();
  void QuitAll();
  void close(int socketNum);
  void DisconnectSocket(int socketNum);

  void setBuffer(char* buffer);
  char* getBuffer();
};

#endif
