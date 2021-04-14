#ifndef _HTTP_UTILS_H_
#define _HTTP_UTILS_H_

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>

#include "MySocket.h"

class MalformedQueryString : public std::runtime_error {
 public:
MalformedQueryString(std::string query) : std::runtime_error("could not parse query string " + query) {}
};

class HttpUtils {
 public:
  static std::map<std::string, std::string> params(std::string query);
  static void writeChunk(MySocket *client, const void *buf, int numBytes);
  static void writeLastChunk(MySocket *client);

  static std::vector<std::string> split(const std::string &s, char delim);

 private:
  static std::vector<std::string> &split(const std::string &s,
					 char delim,
					 std::vector<std::string> &elems);
};

#endif
