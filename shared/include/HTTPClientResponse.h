#ifndef HTTP_CLIENT_REQUEST_H_
#define HTTP_CLIENT_REQUEST_H_

#include "MySocket.h"

#include "rapidjson/document.h"

#include <map>
#include <string>

class HTTPClientResponse {
 public:
  HTTPClientResponse(MySocket *sock);    
  std::string readResponse();
  int status() { return m_status_code; }
  bool success() { return m_status_code >= 200 && m_status_code < 300; }
  std::string body() { return m_body; }
  // make sure to free the document after you're done with it
  rapidjson::Document *jsonBody();
  
 protected:
  MySocket *m_sock;
  std::string m_body;
  std::map<std::string, std::string> m_headers;
  int m_status_code;
  std::string m_status_message;
};

#endif
