#include "HttpClient.h"
#include "HTTPResponse.h"

#include <sstream>

using namespace std;

HttpClient::HttpClient(const char *inetAddr, int port) {
  connection = new MySocket(inetAddr, port);
  
  stringstream host;
  host << inetAddr << ":" << port;
  headers["Host"] = host.str();
  headers["User-Agent"] = string("Gunrock/1.0");
  headers["Accept"] = string("*/*");
}

HttpClient::~HttpClient() {
  delete connection;
}

void HttpClient::write_get_request(string path) {
  stringstream request;
  
  request << "GET " << path << "\r\n";
  map<string, string>::iterator iter;
  for (iter = headers.begin(); iter != headers.end(); iter++) {
    request << iter->first << ": " << iter->second << "\r\n";
  }
  request << "\r\n";

  connection->write(request.str());
}

HTTPResponse *HttpClient::read_response() {
  HTTPResponse *response = new HTTPResponse(connection);
  response->readResponse();
  return response;
}

HTTPResponse *HttpClient::get(string path) {
  write_get_request(path);
  return read_response();
}
