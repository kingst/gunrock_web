#ifndef _DEPOSITSERVICE_H_
#define _DEPOSITSERVICE_H_

#include "HttpService.h"
#include "Database.h"

#include <string>

class DepositService : public HttpService {
 public:
  DepositService();
  virtual void post(HTTPRequest *request, HTTPResponse *response);
};

#endif
