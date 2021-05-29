#ifndef HTTP_SERVICE_H_
#define HTTP_SERVICE_H_

#include <string>
#include <stdexcept>

#include "MySocket.h"
#include "Database.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HttpService {
 public:
  HttpService(std::string pathPrefix);
  std::string pathPrefix();
  
  virtual void head(HTTPRequest *request, HTTPResponse *response);
  virtual void get(HTTPRequest *request, HTTPResponse *response);
  virtual void put(HTTPRequest *request, HTTPResponse *response);
  virtual void post(HTTPRequest *request, HTTPResponse *response);
  virtual void del(HTTPRequest *request, HTTPResponse *response);

  /**
   * A reference to the single in-memory database for wallet data
   */
  Database *m_db;

  /**
   * A helper function for looking up users on authenticated requests.
   *
   * Any API call handlers that require users to be authenticated
   * should use this method to lookup the current user object for the
   * request.
   *
   * @param request the HTTPRequest object that may contain auth info
   * @return the User object for the authenticated user
   * @throws ClientError for any cases where we can't lookup the user
   */
  User *getAuthenticatedUser(HTTPRequest *request);
  
 private:
  std::string m_pathPrefix;
};

#endif
