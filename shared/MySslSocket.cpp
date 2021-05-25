#include "MySslSocket.h"

#include <iostream>
#include <sstream>

#include <openssl/conf.h>
#include <openssl/opensslconf.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <openssl/tls1.h>
#include <openssl/x509v3.h>

// class based on code from:
//   https://wiki.openssl.org/index.php/SSL/TLS_Client
//   https://github.com/microsoft/RIoT/blob/master/Tools/TlsClient/TlsClient.cpp
//   https://gist.github.com/vedantroy/d2b99d774484cf4ea5165b200888e414

// WARNING WARNING WARNING this code doesn't do any certificate
// checking and should not be used in production environments

using namespace std;

void handleFailure() {
  ERR_print_errors_fp(stderr);
  throw SocketError("SSL call failure");
}

MySslSocket::MySslSocket(const char *inetAddr, int port, bool debug_print_io) {
  this->debug_print_io = debug_print_io;
  ctx = NULL;
  ssl = NULL;
  int res;
  
  const SSL_METHOD* method = TLS_client_method();
  if(!(NULL != method)) handleFailure();

  ctx = SSL_CTX_new(method);
  if(!(ctx != NULL)) handleFailure();

  ssl = SSL_new(ctx);
  if (!(ssl != NULL)) handleFailure();
  
  call_connect(inetAddr, port);
  SSL_set_fd(ssl, sockFd);

  res = SSL_connect(ssl);
  if (res != 1) handleFailure();
}

void MySslSocket::write(string buffer) {
  const unsigned char *buf = (const unsigned char *) buffer.c_str();
  unsigned int len = buffer.size();
  int bytesWritten = 0;

  if (sockFd<0 || ssl==NULL) {
    throw SocketNotConnected();
  }

  if (debug_print_io) {
    cout << "MySslSocket::write" << endl;
    cout << "------------------" << endl;
    cout << buffer << endl << endl;
  }

  while(len > 0) {
    bytesWritten = SSL_write(ssl, buf, len);
    if(bytesWritten <= 0) {
      throw SocketWriteError();
    }
    buf += bytesWritten;
    len -= bytesWritten;
  }
}

string MySslSocket::read() {
  char buffer[4096];
  if(sockFd<0 || ssl == NULL) {
    throw SocketNotConnected();
  }
    
  int ret = SSL_read(ssl, buffer, sizeof(buffer));
  
  if(ret <= 0) {
    throw SocketReadError();
  }

  string result = string(buffer, ret);
  
  if (debug_print_io) {
    cout << "MySslSocket::read" << endl;
    cout << "-----------------" << endl;
    cout << result << endl << endl;
  }
  
  return result;
}

void MySslSocket::close() {
  if(NULL != ctx)
    SSL_CTX_free(ctx);

  if (NULL != ssl)
    SSL_free(ssl);

  MySocket::close();
  
  ctx = NULL;
  ssl = NULL;
}
