#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <map>
#include <string>

class HTTPResponse {
 public:
  HTTPResponse();
  void withStreaming();
  void setHeader(std::string name, std::string value);
  void setBody(std::string data);
  void setContentType(std::string contentType);
  void setStatus(int status);
  int getStatus();
  std::string response();

 private:
  std::string statusToString();

  int status;
  bool streaming;
  std::map<std::string, std::string> headers;
  std::string body;
  std::string contentType;
};

#endif
