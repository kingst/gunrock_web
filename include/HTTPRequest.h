#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "MySocket.h"
#include "http_parser.h"
#include "HTTP.h"

#include "WwwFormEncodedDict.h"
#include "StringUtils.h"

#include <map>
#include <string>
#include <vector>

class HTTPRequest {
public:
  HTTPRequest(MySocket *sock, int serverPort);
  ~HTTPRequest();
  
  bool readRequest();

  std::string getHost();
  std::string getRequest();
  std::string getUrl();
  std::string getPath();
  std::vector<std::string> getPathComponents();
  std::string getHeader(std::string key);
  bool hasAuthToken();
  std::string getAuthToken();
  bool isConnect();
  bool isGet() {return m_http->isGet();}
  bool isHead() {return m_http->isHead();}
  bool isPut() {return m_http->isPut();}
  bool isPost() {return m_http->isPost();}
  bool isDelete() {return m_http->isDelete();}
  bool isMove() {return m_http->isMove();}
  std::map<std::string, std::string> getParams();
  WwwFormEncodedDict formEncodedBody();
  std::string getBody() {return m_http->getBody();}
  
  void printDebugInfo();
    
 protected:
    void onRead(const char *buffer, unsigned int len);

    MySocket *m_sock;
    HTTP *m_http;
    int m_serverPort;
    unsigned long m_totalBytesRead;
    unsigned long m_totalBytesWritten;
};

#endif
