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

vector<HttpService> services;

void handle_request(MySocket *client) {
  bool requestActive = true;
  while (requestActive) {
    HTTPRequest *request = new HTTPRequest(client, PORT);
    HTTPResponse *response = new HTTPResponse();

    // XXX FIXME find the path
    FileService *service = new FileService();
    bool readResult = false;
    try {
      readResult = request->readRequest();
    } catch (...) {
      // swallow it
    }

    if (!readResult) {
      delete response;
      delete request;
      break;
    }
    
    if (request->isHead()) {
      cout << "HEAD " << request->getPath() << endl;
      service->head(request, response);
    } else if (request->isGet()) {
      cout << "GET " << request->getPath() << endl;
      service->get(request, response);
    } else {
      // not implemented status
      response->setStatus(405);
    }

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

  while(true) {
    client = server->accept();
    handle_request(client);
  }
}
