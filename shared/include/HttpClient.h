#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <string>
#include <map>

#include "HTTPClientResponse.h"
#include "MySocket.h"

class HttpClient {
 public:
  /**
   * Constructor.
   *
   * The constructor accepts a string representation of and ip address
   * ("192.168.0.1") or domain name ("www.cs.udavis.edu") and
   * connects.  Will throw an HostNotFound exception if the attepted
   * connection fails.
   *
   * Note: this call will block while establishing a connection.
   *
   * @param inetAddr either ip address, or the domain name
   * @param port the port to connect to
   */
  HttpClient(const char *inet_addr, int port, bool use_tls=false);
  ~HttpClient();


  /**
   * Establishes basic auth header
   *
   * Crafts your username and password into an Authorization header,
   * following the HTTP spec.
   *
   * @param username the username that you want to use
   * @param password the password for this username
   */
  void set_basic_auth(std::string username, std::string password);

  /**
   * HTTP GET request
   *
   * Makes a network call using the HTTP GET method.
   *
   * @param path the API endpoint that you want to connect to
   * @return HTTPClientResponse a pointer to a client response
   *         object, hydrated from the API server.
   */
  HTTPClientResponse *get(std::string path);

  /**
   * HTTP POST request
   *
   * Makes a network call using the HTTP POST method.
   *
   * @param path the API endpoint that you want to connect to
   * @param body the message body to include along with the POST
   * @return HTTPClientResponse a pointer to a client response
   *         object, hydrated from the API server.
   */
  HTTPClientResponse *post(std::string path, std::string body);

  /**
   * HTTP PUT request
   *
   * Makes a network call using the HTTP PUT method.
   *
   * @param path the API endpoint that you want to connect to
   * @param body the message body to include along with the POST
   * @return HTTPClientResponse a pointer to a client response
   *         object, hydrated from the API server.
   */
  HTTPClientResponse *put(std::string path, std::string body);

  /**
   * HTTP DELETE request
   *
   * Makes a network call using the HTTP DELETE method.
   *
   * @param path the API endpoint that you want to connect to
   * @return HTTPClientResponse a pointer to a client response
   *         object, hydrated from the API server.
   */
  HTTPClientResponse *del(std::string path);

  /**
   * Set a header key/value pair
   *
   * Set a header value _before_ you issue a get or post request and
   * this class will include that header along with the HTTP request.
   *
   * @param key the key for the header
   * @param value the value for the header with key
   */
  void set_header(std::string key, std::string value);
  
  void write_request(std::string path, std::string method, std::string body);
  HTTPClientResponse *read_response();
  
 private:
  MySocket *connection;
  std::map<std::string, std::string> headers;
};
  

#endif
