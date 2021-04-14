#ifndef HTTP_SERVICE_H_
#define HTTP_SERVICE_H_

#include <string>

#include "MySocket.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HttpService {
 public:
  HttpService(std::string pathPrefix);
  std::string pathPrefix();
  virtual ~HttpService();

  virtual void head(HTTPRequest *request, HTTPResponse *response);
  virtual void get(HTTPRequest *request, HTTPResponse *response);

 private:
  std::string m_pathPrefix;
};

#endif
