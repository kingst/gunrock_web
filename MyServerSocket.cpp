#include "MyServerSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MyServerSocket::MyServerSocket(int port)
{
    struct sockaddr_in server;
    int one = 1;
  
    // set up the server socket
    serverFd = socket(AF_INET,SOCK_STREAM,0);
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons((short) port);
    
    if (setsockopt(serverFd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(int)) == -1) {
      throw SocketError("error with set socket opts");
    }
    
    if( bind(serverFd,(struct sockaddr *) &server, sizeof(server)) ==-1){
        char str[1024];
        snprintf(str, 1023, "could not bind to port %d",port);
        throw SocketError(str);
    }	
    
    //set up a listen queue
    listen(serverFd, 10);
}

MySocket *MyServerSocket::accept()
{
    //check that the sockFd is valid
    
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int clientFd = ::accept(serverFd, (struct sockaddr *) &client, &len);
    
    if(clientFd<0) {
      throw SocketError("accept error");
    }
    
    return new MySocket(clientFd);
}
