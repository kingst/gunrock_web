#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <stdexcept>
#include <string>

class SocketNotConnected : public std::runtime_error {
 public:
  SocketNotConnected() : std::runtime_error("socket not connected") {}
};

class SocketWriteError : public std::runtime_error {
 public:
  SocketWriteError() : std::runtime_error("socket write error") {}
};

class SocketReadError : public std::runtime_error {
 public:
  SocketReadError() : std::runtime_error("socket read error") {}
};

class SocketError : public std::runtime_error {
 public:
  SocketError(std::string err) : std::runtime_error("socket error: " + err) {}
};

class MySocket {
 public:
  /*
   * this is the constructor.  It accepts a string representation of
   * and ip address ("192.168.0.1") or domain name ("www.cs.uiuc.edu")
   * and connects.  Will throw an HostNotFound exception if the attepted
   * connection fails.  MySocket uses the TCP protocol.
   *
   * @param inetAddr either ip address, or the domain name
   * @param port the port to connect to
   */
  MySocket(const char *inetAddr, int port);

  /*
   * this constructor will generally not be used except for by ServerSockets
   */
  MySocket(int socketFileDesc);

  /*
   * default constructor, makes sure the state is properly specified
   */
  MySocket(void);
  virtual ~MySocket(void);


  virtual std::string read();
  virtual void write(std::string data);
  virtual void close(void);
  
 protected:
  void call_connect(const char *inetAddr, int port);
  void write_bytes(const void *buffer, int len);
  int sockFd;
};

#endif
