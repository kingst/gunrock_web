#ifndef MYSERVERSOCKET_H
#define MYSERVERSOCKET_H

#include <stdexcept>
#include <string>

#include "MySocket.h"

class MyServerSocket {
 public:
  /**
   * creates a new server socket and binds it to the port specified.
   * if it cannot bind, it will throw a socket exception.
   *
   * @param port the port to bind to
   */
  MyServerSocket(int port);
  MyServerSocket() { serverFd = -1; }
  
  /**
   * this function will accept incoming requests to connect and
   * return the resulting socket
   */
  MySocket *accept();

  int getFd() { return serverFd; }
 protected:
  int serverFd;

};


#endif
