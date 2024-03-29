#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>

#include "DistributedFileSystemService.h"
#include "ClientError.h"

using namespace std;

DistributedFileSystemService::DistributedFileSystemService(string diskFile) : HttpService("/ds3/") {
 this->m_diskFile = diskFile;
}


void DistributedFileSystemService::get(HTTPRequest *request, HTTPResponse *response) {
  response->setBody("DistributedFileSystemService\r\n");
}

void DistributedFileSystemService::put(HTTPRequest *request, HTTPResponse *response) {
  response->setBody("");
}

void DistributedFileSystemService::del(HTTPRequest *request, HTTPResponse *response) {
  response->setBody("");
}

void DistributedFileSystemService::move(HTTPRequest *request, HTTPResponse *response) {
  response->setBody("");
}
