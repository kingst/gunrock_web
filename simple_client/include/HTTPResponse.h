#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "MySocket.h"

#include <map>
#include <string>

class HTTPResponse {
 public:
  HTTPResponse(MySocket *sock);    
  std::string readResponse();
  int status() { return m_status_code; }
  std::string body() { return m_body; }
  
 protected:
  MySocket *m_sock;
  std::string m_body;
  std::map<std::string, std::string> m_headers;
  int m_status_code;
  std::string m_status_message;
};

#endif
