#include "./tcp/mars_tcp.h"
void ServerFunction(int socketNum, char* buffer, char* checkStr, Tcp* serverTcp);
void GetString(char* str);
void EndProcess(char* buffer, char* checkStr, Tcp* serverTcp);

int main(void){
  char* buffer = new char[BUFFERSIZE];
  char* checkStr = new char[BUFFERSIZE];
  int listeningSocketNum, newSocketNum;
  pid_t sendpid, recvpid[MAXCLIENT];

  //Connect TCP
  Tcp serverTcp;
  listeningSocketNum = serverTcp.BuildServerTCP();

  sendpid = fork(); //fork send process

  while(true){
    int newSocketNum = serverTcp.FindEmptySocket();
    serverTcp.ListeningClient(newSocketNum); //Parent Process, Listening client socket

    //if new socket is connected, update send process
    kill(sendpid, SIGINT);
    if((sendpid=fork()) == -1){//Check error
      serverTcp.close(listeningSocketNum);
      perror("fork() error\n");
      EndProcess(buffer, checkStr, &serverTcp);
      break;
    }
    else if(sendpid == 0 ){//Child process, Send message to client using write system call.
      while(true){
        GetString(checkStr);
        if(atoi(checkStr) == 0){
          if(strncmp(checkStr, "quit", 4) == 0){ //shut down server
            ServerFunction(listeningSocketNum, buffer, checkStr, &serverTcp);
            break;
          }
          else if(strncmp(checkStr, "print", 5) == 0){ //show connected sockets
            serverTcp.CheckConnectedSocket();
          }
          else{
            printf("Please input socket number correctly(1~MAX)\n");
          }
        }
        else{ //send msg to specific socket
          printf("send msg to %d socket : ", atoi(checkStr));
          GetString(buffer);
          serverTcp.WriteMsg(checkStr ,buffer);

          ServerFunction(newSocketNum, buffer, checkStr, &serverTcp); //Server functions
        }
      }
      break;
    }

    if((recvpid[newSocketNum]=fork()) == -1){//Check error
      serverTcp.close(listeningSocketNum);
      perror("fork() error\n");
      EndProcess(buffer, checkStr, &serverTcp);
    }
    else if(recvpid[newSocketNum] == 0){//Child process, Print the message and perform functions
      while(true){
        strcpy(buffer, serverTcp.ReadMsg(newSocketNum, SERVER));
        printf("Socket Num(%d) : %s", newSocketNum, buffer);

        ServerFunction(newSocketNum, buffer, checkStr, &serverTcp); //Server functions
      }
    }
  }
  kill(getppid(), SIGINT); //Part of "quit" function
  return 0;
}

//////////////////////////////////////////////////////////////////////////////////
/*If you want to make function which is executed in Server, modify this function*/
//////////////////////////////////////////////////////////////////////////////////
void ServerFunction(int socketNum, char* buffer, char* checkStr, Tcp* serverTcp){
  if(strncmp(buffer, "exit", 4) == 0){
    strcpy(buffer, "stopRecv");
    serverTcp->WriteMsg(socketNum, buffer, SERVER);
    delete[] buffer;
    delete[] checkStr;
    serverTcp->DisconnectSocket(socketNum);
    exit(0);
  }
  else if(strncmp(buffer, "stopRecv", 8) == 0){
    printf("\n");
    serverTcp->DisconnectSocket(socketNum);
    delete[] buffer;
    delete[] checkStr;
    exit(0);
  }
  else if(strncmp(checkStr, "quit", 4) == 0){
    printf("Shut down the server\n");
    serverTcp->QuitAll();
    EndProcess(buffer, checkStr, serverTcp);
  }
  // else if(strncmp(checkStr, "FUNCTION NAME", length(FUNCTION NAME)) == 0){
  //   DO you want
  // }
}
//////////////////////////////////////////////////////////////////////////////////

void GetString(char* str){ //get input buffer
  bzero(str, sizeof(str));
  fflush(stdin);
  fgets(str, BUFFERSIZE ,stdin);
}

void EndProcess(char* buffer, char* checkStr, Tcp* serverTcp){ //release memory and quit server
  delete[] buffer;
  delete[] checkStr;
  serverTcp->QuitTcp();
}
