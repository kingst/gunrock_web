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

vector<HttpService *> services;

void handle_request(MySocket *client) {
  while (true) {
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
      break;
    }

    // find a service that is registered for this path prefix
    HttpService *service = NULL;
    for (int idx = 0; idx < services.size(); idx++) {
      if (request->getPath().find(services[idx]->pathPrefix()) == 0) {
	service = services[idx];
	break;
      }
    }

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

    // send data back to the client and clean up
    cout << "RESPONSE " << response->getStatus() << endl;
    client->write(response->response());
    
    delete response;
    delete request;
  }

  cout << "closing connection" << endl;
  client->close();
  delete client;
}

int main(int /*argc*/, char */*argv*/[]) {

  signal(SIGPIPE, SIG_IGN);

  MyServerSocket *server = new MyServerSocket(PORT);
  MySocket *client;

  services.push_back(new FileService());
  
  while(true) {
    client = server->accept();
    handle_request(client);
  }
}
