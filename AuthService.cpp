#define RAPIDJSON_HAS_STDSTRING 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AuthService.h"
#include "StringUtils.h"
#include "ClientError.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

AuthService::AuthService() : HttpService("/auth-tokens") {
  
}

void AuthService::post(HTTPRequest *request, HTTPResponse *response) {

}

void AuthService::del(HTTPRequest *request, HTTPResponse *response) {

}
