#include <sstream>

#include "HTTPResponse.h"

using namespace std;

HTTPResponse::HTTPResponse() {
  this->streaming = false;
  this->contentType = "text/html; charset=ISO-8859-1";
  this->headers["Server"] = "Gunrock Web";
  this->status = 200;
}

void HTTPResponse::withStreaming() {
  this->streaming = true;
}

void HTTPResponse::setHeader(string name, string value) {
  this->headers[name] = value;
}

void HTTPResponse::setBody(string data) {
  body = data;
}

int HTTPResponse::getStatus() {
  return status;
}

void HTTPResponse::setContentType(string contentType) {
  this->contentType = contentType;
}

void HTTPResponse::setStatus(int status) {
  this->status = status;
}

string HTTPResponse::statusToString() {
  if (status == 200) {
    return "OK";
  } else {
    return "Unknown";
  }
}

string HTTPResponse::response() {
  stringstream out;
  setHeader("Content-Type", contentType);
  if (streaming) {
    setHeader("Transfer-Encoding", "chunked");
  } else {
    stringstream len;
    len << body.size();
    setHeader("Content-Length", len.str());
  }

  out << "HTTP/1.1 " << status << " " << statusToString() << "\r\n";
  map<string, string>::iterator iter;
  for(iter = headers.begin(); iter != headers.end(); iter++) {
    out << iter->first << ": " << iter->second << "\r\n";
  }
  out << "\r\n";
  if (body.size() > 0 && !streaming) {
    out << body;
  }

  return out.str();
}
