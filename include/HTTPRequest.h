#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "MySocket.h"
#include "http_parser.h"
#include "HTTP.h"

#include <map>
#include <string>

class HTTPRequest {
 public:
    HTTPRequest(MySocket *sock, int serverPort);
    ~HTTPRequest();
    
    bool readRequest();

    std::string getHost();
    std::string getRequest();
    std::string getUrl();
    std::string getPath();
    bool isConnect();
    bool isGet() {return m_http->isGet();}
    bool isHead() {return m_http->isHead();}
    std::map<std::string, std::string> getParams();

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
