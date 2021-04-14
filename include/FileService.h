#ifndef _FILESERVICE_H_
#define _FILESERVICE_H_

#include "HttpService.h"

#include <string>

class FileService : public HttpService {
 public:
  FileService();
  virtual ~FileService();

  virtual void get(HTTPRequest *request, HTTPResponse *response);
  virtual void head(HTTPRequest *request, HTTPResponse *response);

private:
  bool endswith(std::string str, std::string suffix);
  std::string readFile(std::string paht);
};

#endif
