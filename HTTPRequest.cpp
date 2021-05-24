#include "HTTPRequest.h"

#include <iostream>
#include <string>

#include <assert.h>
#include <errno.h>

#include "HttpUtils.h"
#include "StringUtils.h"

using namespace std;

#define CONNECT_REPLY "HTTP/1.1 200 Connection Established\r\n\r\n"

HTTPRequest::HTTPRequest(MySocket *sock, int serverPort)
{
    m_sock = sock;
    m_http = new HTTP();
    m_serverPort = serverPort;
    m_totalBytesRead = 0;
    m_totalBytesWritten = 0;
}

HTTPRequest::~HTTPRequest()
{
    delete m_http;
}

void HTTPRequest::printDebugInfo()
{
    cerr << "    isDone = " << m_http->isDone() << endl;
    cerr << "    bytesRead = " << m_totalBytesRead << endl;
    cerr << "    bytesWritte = " << m_totalBytesWritten << endl;
    cerr << "    url = " << m_http->getUrl() << endl;
}

map<string, string> HTTPRequest::getParams() {
  return HttpUtils::params(m_http->getQuery());
}

WwwFormEncodedDict HTTPRequest::formEncodedBody() {
  WwwFormEncodedDict dict(m_http->getBody());
  return dict;
}

string HTTPRequest::getPath() {
  return m_http->getPath();
}

string HTTPRequest::getHeader(string key) {
  vector<pair<string *, string *> >::iterator iter;
  vector<pair<string *, string *> > headers = m_http->getHeaders();
  for (iter = headers.begin(); iter != headers.end(); iter++) {
    string header_key = *(iter->first);
    if (header_key == key) {
      return *(iter->second);
    }
  }

  throw "could not find header";
}

bool HTTPRequest::hasAuthToken() {
  try {
    getHeader("x-auth-token");
    return true;
  } catch (...) {
    return false;
  }
}

string HTTPRequest::getAuthToken() {
  try {
    return getHeader("x-auth-token");
  } catch (...) {
    return "";
  }
}

vector<string> HTTPRequest::getPathComponents() {
  return StringUtils::split(getPath(), '/');
}

bool HTTPRequest::readRequest()
{
    assert(!m_http->isDone());

    string readData;
    while(!m_http->isDone()) {
        readData = m_sock->read();
	onRead(readData.c_str(), readData.size());
    }

    return true;
}

void HTTPRequest::onRead(const char *buffer, unsigned int len)
{
    m_totalBytesRead += len;

    unsigned int bytesRead = 0;
    assert(len > 0);

    while(bytesRead < len) {
        assert(!m_http->isDone());
        int ret = m_http->addData((const unsigned char *) (buffer + bytesRead), len - bytesRead);
        assert(ret > 0);
        bytesRead += ret;
        
        // This is a workaround for a parsing bug that sometimes
        // crops up with connect commands.  The parser will think
        // it is done before it reads the last newline of some
        // properly formatted connect requests
        if(m_http->isDone() && (bytesRead < len)) {
            if(m_http->isConnect() && ((len-bytesRead) == 1) && (buffer[bytesRead] == '\n')) {
                break;
            } else {
                assert(false);
            }
        }
    }
}

string HTTPRequest::getHost()
{
    return m_http->getHost();
}
string HTTPRequest::getRequest()
{
    return m_http->getProxyRequest();
}
string HTTPRequest::getUrl()
{
    return m_http->getUrl();
}

bool HTTPRequest::isConnect()
{
    return m_http->isConnect();
}
