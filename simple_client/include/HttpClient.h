#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <string>
#include <map>

#include "HTTPResponse.h"
#include "MySocket.h"

class HttpClient {
 public:
  /*
   * this is the constructor.  It accepts a string representation of
   * and ip address ("192.168.0.1") or domain name ("www.cs.uiuc.edu")
   * and connects.  Will throw an HostNotFound exception if the attepted
   * connection fails.
   *
   * @param inetAddr either ip address, or the domain name
   * @param port the port to connect to
   */
  HttpClient(const char *inetAddr, int port);
  ~HttpClient();
  HTTPResponse *get(std::string path);
  void write_get_request(std::string path);
  HTTPResponse *read_response();
  
 private:
  MySocket *connection;
  std::map<std::string, std::string> headers;
};
  

#endif
