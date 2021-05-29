#ifndef MYSSLSOCKET_H
#define MYSSLSOCKET_H

#include "MySocket.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>

class MySslSocket: public MySocket {
 public:
  /**
   * this is the constructor.  It accepts a string representation of
   * and ip address ("192.168.0.1") or domain name ("www.cs.ucdavis.edu")
   * and connects.  Will throw an HostNotFound exception if the attepted
   * connection fails.  MySslSocket uses the TLS/SSL protocol over TCP.
   *
   * @param inetAddr either ip address, or the domain name
   * @param port the port to connect to
   * @param debug_print_io print the cleartext I/O on this socket
   */
  MySslSocket(const char *inetAddr, int port, bool debug_print_io=false);

  std::string read();
  void write(std::string data);
  void close(void);
  
 protected:
  SSL_CTX *ctx;
  SSL *ssl;
  bool debug_print_io;
};

#endif
