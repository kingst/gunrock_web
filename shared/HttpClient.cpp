#include "HttpClient.h"
#include "HTTPClientResponse.h"
#include "MySslSocket.h"
#include "Base64.h"

#include <sstream>

using namespace std;

HttpClient::HttpClient(const char *inet_addr, int port, bool use_tls) {
  if (use_tls) {
    connection = new MySslSocket(inet_addr, port);
  } else {
    connection = new MySocket(inet_addr, port);
  }
  
  stringstream host;
  host << inet_addr << ":" << port;
  headers["Host"] = host.str();
  headers["User-Agent"] = string("Gunrock/1.0");
  headers["Accept"] = string("*/*");
  headers["Connection"] = string("close");
}

HttpClient::~HttpClient() {
  delete connection;
}

void HttpClient::set_header(string key, string value) {
  headers[key] = value;
}

void HttpClient::set_basic_auth(string username, string password) {
  string user_pass = username + ":" + password;
  string value = "Basic " + Base64::bytesToBase64((const unsigned char *) user_pass.c_str(),
						  user_pass.size());
  set_header("Authorization", value);
}

void HttpClient::write_request(string path, string method, string body) {
  stringstream request;

  // PART 1: implement support for handling the body, if it exists
  request << method << " " << path << " HTTP/1.1\r\n";
  if (body.size() > 0) {
    stringstream length;
    length << body.size();
    headers["Content-Length"] = length.str();
  } else {
    headers.erase("Content-Length");
  }
  
  map<string, string>::iterator iter;
  for (iter = headers.begin(); iter != headers.end(); iter++) {
    request << iter->first << ": " << iter->second << "\r\n";
  } 
  
  request << "\r\n";

  if (body.size() > 0) {
    request << body;
  }
  
  connection->write(request.str());
}



HTTPClientResponse *HttpClient::read_response() {
  HTTPClientResponse *response = new HTTPClientResponse(connection);
  response->readResponse();
  return response;
}

HTTPClientResponse *HttpClient::get(string path) {
  write_request(path, "GET", "");
  return read_response();
}

HTTPClientResponse *HttpClient::post(string path, string body) {
  write_request(path, "POST", body);
  return read_response();
}

HTTPClientResponse *HttpClient::put(string path, string body) {
  write_request(path, "PUT", body);
  return read_response();
}

HTTPClientResponse *HttpClient::del(string path) {
  write_request(path, "DELETE", "");
  return read_response();
}
