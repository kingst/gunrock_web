#include "MySocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>

#include <iostream>

using namespace std;

MySocket::MySocket(const char *inetAddr, int port) {
  call_connect(inetAddr, port);
}

void MySocket::call_connect(const char *inetAddr, int port) {
    struct sockaddr_in server;
    struct addrinfo hints;
    struct addrinfo *res;

    // set up the new socket (TCP/IP)
    sockFd = socket(AF_INET,SOCK_STREAM,0);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(inetAddr, NULL, &hints, &res);
    if(ret != 0) {
        string str;
        str = string("Could not get host ") + string(inetAddr);
        throw SocketError(str.c_str());
    }
    
    server.sin_addr = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
    server.sin_port = htons((short) port);
    server.sin_family = AF_INET;
    freeaddrinfo(res);
    
    // conenct to the server
    if( connect(sockFd, (struct sockaddr *) &server,
                sizeof(server)) == -1 ) {
        throw SocketError("Did not connect to the server");
    }
}

MySocket::MySocket(void) {
    sockFd = -1;
}

MySocket::MySocket(int socketFileDesc) {
    sockFd = socketFileDesc;
}

MySocket::~MySocket(void) {
    close();
}


void MySocket::write(string buffer) {
    write_bytes(buffer.c_str(), buffer.size());
}

void MySocket::write_bytes(const void *buffer, int len) {
    const unsigned char *buf = (const unsigned char *) buffer;
    int bytesWritten = 0;

    if (sockFd<0) {
      throw SocketNotConnected();
    }

    while(len > 0) {
        bytesWritten = ::write(sockFd, buf, len);
        if(bytesWritten <= 0) {
	  throw SocketWriteError();
        }
        buf += bytesWritten;
        len -= bytesWritten;
    }
}

string MySocket::read() {
    char buffer[4096];
    if(sockFd<0) {
      throw SocketNotConnected();
    }
    
    int ret = ::read(sockFd, buffer, sizeof(buffer));
    
    if(ret <= 0) {
      throw SocketReadError();
    }
  
    return string(buffer, ret);
}

void MySocket::close(void) {
    if(sockFd<0) return;
    
    ::close(sockFd);

    sockFd = -1;
}
