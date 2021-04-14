#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>

#include "FileService.h"

using namespace std;

FileService::FileService() : HttpService("/") {

}

FileService::~FileService() {

}

void FileService::get(HTTPRequest *request, HTTPResponse *response) {
  string path = "static" + request->getPath();
  string fileContents = this->readFile(path);
  if (fileContents.size() == 0) {
    response->setStatus(404);
  } else {
    response->setBody(fileContents);
  }
}

string FileService::readFile(string path) {
  int fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    return "";
  }

  string result;
  int ret;
  char buffer[4096];
  while ((ret = read(fd, buffer, sizeof(buffer))) > 0) {
    result.append(buffer, ret);
  }

  close(fd);
  
  return result;
}

void FileService::head(HTTPRequest *request, HTTPResponse *response) {
  string path = request->getPath();
  cout << "file service head path " << path << endl;
}
