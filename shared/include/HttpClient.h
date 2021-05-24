#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <string>
#include <map>

#include "HTTPClientResponse.h"
#include "MySocket.h"

class HttpClient {
 public:
  /*
   * this is the constructor.  It accepts a string representation of
   * and ip address ("192.168.0.1") or domain name ("www.cs.udavis.edu")
   * and connects.  Will throw an HostNotFound exception if the attepted
   * connection fails.
   *
   * @param inetAddr either ip address, or the domain name
   * @param port the port to connect to
   */
  HttpClient(const char *inet_addr, int port, bool use_tls=false);
  ~HttpClient();

  void set_basic_auth(std::string username, std::string password);
  
  HTTPClientResponse *get(std::string path);
  HTTPClientResponse *post(std::string path, std::string body);
  void write_request(std::string path, std::string method, std::string body);

  HTTPClientResponse *read_response();
  void set_header(std::string key, std::string value);
  
 private:
  MySocket *connection;
  std::map<std::string, std::string> headers;
};
  

#endif
