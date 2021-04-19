#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HttpService.h"
#include "HttpUtils.h"
#include "FileService.h"
#include "MySocket.h"
#include "MyServerSocket.h"

using namespace std;

int PORT = 8080;
int THREAD_POOL_SIZE = 1;
int BUFFER_SIZE = 1;
string BASEDIR = "static";
string SCHEDALG = "FIFO";

vector<HttpService *> services;

HttpService *find_service(HTTPRequest *request) {
   // find a service that is registered for this path prefix
  for (unsigned int idx = 0; idx < services.size(); idx++) {
    if (request->getPath().find(services[idx]->pathPrefix()) == 0) {
      return services[idx];
    }
  }

  return NULL;
}


void invoke_service_method(HttpService *service, HTTPRequest *request, HTTPResponse *response) {
  // invoke the service if we found one
  if (service == NULL) {
    // not found status
    response->setStatus(404);
  } else if (request->isHead()) {
    cout << "HEAD " << request->getPath() << endl;
    service->head(request, response);
  } else if (request->isGet()) {
    cout << "GET " << request->getPath() << endl;
    service->get(request, response);
  } else {
    // not implemented status
    response->setStatus(405);
  }
}

/**
 * Note: in a real web server you wouldn't close the connection
 * after reading a single request but to keep the queuing logic
 * simple we'll do it this way.
 */
void handle_request(MySocket *client) {
  HTTPRequest *request = new HTTPRequest(client, PORT);
  HTTPResponse *response = new HTTPResponse();

  // read in the request
  bool readResult = false;
  try {
    readResult = request->readRequest();
  } catch (...) {
    // swallow it
  }    
    
  if (!readResult) {
    // there was a problem reading in the request, bail
    delete response;
    delete request;
    return;
  }

  HttpService *service = find_service(request);
  invoke_service_method(service, request, response);

  // send data back to the client and clean up
  cout << "RESPONSE " << response->getStatus() << endl;
  client->write(response->response());
    
  delete response;
  delete request;

  cout << "closing connection" << endl;
  client->close();
  delete client;
}

int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);
  int option;

  while ((option = getopt(argc, argv, "d:p:t:b:s:")) != -1) {
    switch (option) {
    case 'd':
      BASEDIR = string(optarg);
      break;
    case 'p':
      PORT = atoi(optarg);
      break;
    case 't':
      THREAD_POOL_SIZE = atoi(optarg);
      break;
    case 'b':
      BUFFER_SIZE = atoi(optarg);
      break;
    case 's':
      SCHEDALG = string(optarg);
      break;
    default:
      cerr<< "usage: " << argv[0] << " [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]" << endl;
      exit(1);
    }
  }
  
  MyServerSocket *server = new MyServerSocket(PORT);
  MySocket *client;

  services.push_back(new FileService(BASEDIR));
  
  while(true) {
    cout << "listening for new connections on " << PORT << endl;
    client = server->accept();
    handle_request(client);
  }
}
