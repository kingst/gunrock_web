#include <iostream>

#include <stdlib.h>
#include <stdio.h>

#include "HttpService.h"

using namespace std;

HttpService::HttpService(string pathPrefix) {
  this->m_pathPrefix = pathPrefix;
}

HttpService::~HttpService() {

}

void HttpService::head(HTTPRequest *request, HTTPResponse *response) {
  cout << "HEAD " << request->getPath() << endl;
}

void HttpService::get(HTTPRequest *request, HTTPResponse *response) {
  cout << "GET " << request->getPath() << endl;
}
