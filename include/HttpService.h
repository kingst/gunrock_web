#ifndef HTTP_SERVICE_H_
#define HTTP_SERVICE_H_

#include <string>
#include <stdexcept>

#include "MySocket.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HttpService {
 public:
  HttpService(std::string pathPrefix);
  std::string pathPrefix();
  
  virtual void head(HTTPRequest *request, HTTPResponse *response);
  virtual void get(HTTPRequest *request, HTTPResponse *response);
  virtual void put(HTTPRequest *request, HTTPResponse *response);
  virtual void post(HTTPRequest *request, HTTPResponse *response);
  virtual void del(HTTPRequest *request, HTTPResponse *response);
  virtual void move(HTTPRequest *request, HTTPResponse *response);
  
 private:
  std::string m_pathPrefix;
};

#endif
