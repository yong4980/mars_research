#include "mars_tcp.h"

Tcp::Tcp(){
  buffer = new char[1024];
  hostIP = new char[20];
  bzero(buffer, sizeof(buffer));
  bzero(hostIP, sizeof(hostIP));

  connectedSocket = new int[MAXSOCKET];
  for(int i=0; i<MAXSOCKET; i++){
    connectedSocket[i] = 0;
  }
}

Tcp::~Tcp(){
}

int Tcp::BuildServerTCP(){
  //shared memory
  key = ftok("shmArray", 123); //Ftok to generate unique key
  shmid = shmget(key, 1024, 0666|IPC_CREAT); //Shmget(make shm) returns an identifier in shmid
  connectedSocket = (int*) shmat(shmid, (void*)0, 0); //Shmat to attach to shared memory

  connectedSocket[0] = socket(AF_INET, SOCK_STREAM, 0); //listeningSocket
  //Check creating socket
  if(connectedSocket[0] < 0){
    printf("[-]Error in connection.\n");
    exit(1);
  }
  printf("[+]Server Socket is created.\n");

  //Set server info
  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons( PORT );
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //Bind
  check = bind(connectedSocket[0], (struct sockaddr*)&serverAddr, sizeof(serverAddr));
  if(check < 0){
    printf("[-]Error in binding.\n");
    exit(1);
  }
  printf("[+]Bind\n");

  //Listening
  if(listen(connectedSocket[0], 10)==0){
    printf("Listening....\n");
  }
  else{
    printf("[-]Error in binding.\n");
  }

  return 0;
}

int Tcp::BuildClientTCP(char serverIP[]){
  strcpy(hostIP, serverIP);
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  //Check creating socket
  if(clientSocket < 0){
		printf("[-]Error in Connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

  //Set server info
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(hostIP);

  //Connect
  check = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(check < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

  return clientSocket;
}

void Tcp::ListeningClient(int newSocketNum){
  int newSocket = accept(connectedSocket[0], (struct sockaddr*)&newClientAddr, &addr_size); //waiting client socket
  connectedSocket[newSocketNum] = newSocket;
  if(newSocket < 0){
    exit(1);
  }
  printf("[+]Connection accepted from %s:%d\n", inet_ntoa(newClientAddr.sin_addr), ntohs(newClientAddr.sin_port));
}

char* Tcp::ReadMsg(int socket, int mode){ //read msg
  int nbytes;
  bzero(buffer, sizeof(buffer));
  if(mode==SERVER){
    nbytes = read(connectedSocket[socket],buffer,1023);
  }
  else if(mode==CLIENT){
    nbytes = read(socket,buffer,1023);
  }

  if(nbytes < 0){
      perror("read() error\n");
      exit(0);
  }
  return buffer;
}


void Tcp::WriteMsg(int socket, char* msg, int mode){ //write msg
  if(mode==SERVER){
    write(connectedSocket[socket], msg, 1023);
  }
  else if(mode==CLIENT){
    write(socket, msg, 1023);
  }
}

void Tcp::WriteMsg(char* socketNumStr, char* msg){ //write msg, function overloading
  int socketNum = atoi(socketNumStr);
  if(connectedSocket[socketNum] == 0){
    printf("That socket is not connected\n");
  }
  else{
    write(connectedSocket[socketNum], msg, 1023);
  }

}

int Tcp::FindEmptySocket(){ //find empty socket
  for(int i=1; i<MAXSOCKET; i++){
    if(connectedSocket[i] == 0){
      return i;
    }
  }
  return -1; //Full
}

void Tcp::CheckConnectedSocket(){ //show connected sockets
  int cnt = 0;
  printf("Connected Socket num : ");
  for(int i=1; i<MAXSOCKET; i++){
    if(connectedSocket[i] != 0){
      printf("%d ", i);
      cnt++;
    }
  }
  if(cnt==0){
    printf("None\n");
  }
  else{
    printf("\n");
  }
}

void Tcp::QuitTcp(){ //detach and delete shared memory, release memory
  shmdt(connectedSocket);
  shmctl(shmid,IPC_RMID,NULL);
  delete[] buffer;
  delete[] hostIP;
  printf("Complete detach shared memory\n");
}

void Tcp::QuitAll(){ //terminate connected sockets
  strcpy(buffer, "stop");
  for(int i=1; i<MAXSOCKET; i++){
    if(connectedSocket[i] != 0){
      WriteMsg(i, buffer, SERVER);
      connectedSocket[i] = 0;
    }
  }
  connectedSocket[0] = 0;
  printf("Disconnect all clients\n");
}

void Tcp::close(int socketNum){ //close socket
  close(connectedSocket[socketNum]);
}

void Tcp::DisconnectSocket(int socketNum){ //remove socket information
  connectedSocket[socketNum] = 0;
}
