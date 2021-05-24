#ifndef _CLIENT_ERROR_H_
#define _CLIENT_ERROR_H_

#include <stdexcept>
#include <string>

class ClientError : public std::runtime_error {
 public:
  ClientError(std::string text, int status_code) : std::runtime_error(text) {
    this->status_code = status_code;
  }
  int status_code;

  static ClientError badRequest() { return ClientError("Bad Request", 400); }
  static ClientError unauthorized() { return ClientError("Unauthorized", 401); }
  static ClientError forbidden() { return ClientError("Forbidden", 403); }
  static ClientError notFound() { return ClientError("Not Found", 404); }
  static ClientError methodNotAllowed() { return ClientError("Method Not Allowed", 405); }
};

#endif
