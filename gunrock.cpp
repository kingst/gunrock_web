#define RAPIDJSON_HAS_STDSTRING 1

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <deque>

#include "ClientError.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "HttpService.h"
#include "HttpUtils.h"
#include "AccountService.h"
#include "AuthService.h"
#include "DepositService.h"
#include "FileService.h"
#include "TransferService.h"
#include "MySocket.h"
#include "MyServerSocket.h"
#include "dthread.h"

#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

int PORT = 8080;
int THREAD_POOL_SIZE = 1;
int BUFFER_SIZE = 1;
string BASEDIR = "static";
string SCHEDALG = "FIFO";
string LOGFILE = "/dev/null";

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
  stringstream payload;

  try {
    // invoke the service if we found one
    if (service == NULL) {
      // not found status
      response->setStatus(404);
    } else if (request->isHead()) {
      service->head(request, response);
    } else if (request->isGet()) {
      service->get(request, response);
    } else if (request->isPut()) {
      service->put(request, response);
    } else if (request->isPost()) {
      service->post(request, response);
    } else if (request->isDelete()) {
      service->del(request, response);
    } else {
      // The server doesn't know about this method
      response->setStatus(501);
    }
  } catch (ClientError &ce) {
    response->setStatus(ce.status_code);
  } catch (...) {
    // reset the response object and return an error
    response->setBody("");
    response->setStatus(500);
  }
}

void handle_request(MySocket *client) {
  HTTPRequest *request = new HTTPRequest(client, PORT);
  HTTPResponse *response = new HTTPResponse();
  stringstream payload;
  
  // read in the request
  bool readResult = false;
  try {
    payload << "client: " << (void *) client;
    sync_print("read_request_enter", payload.str());
    readResult = request->readRequest();
    sync_print("read_request_return", payload.str());
  } catch (...) {
    // swallow it
  }    
    
  if (!readResult) {
    // there was a problem reading in the request, bail
    delete response;
    delete request;
    sync_print("read_request_error", payload.str());
    return;
  }
  
  HttpService *service = find_service(request);
  invoke_service_method(service, request, response);

  // send data back to the client and clean up
  payload.str(""); payload.clear();
  payload << " RESPONSE " << response->getStatus() << " client: " << (void *) client;
  sync_print("write_response", payload.str());
  cout << payload.str() << endl;
  client->write(response->response());
    
  delete response;
  delete request;

  payload.str(""); payload.clear();
  payload << " client: " << (void *) client;
  sync_print("close_connection", payload.str());
  client->close();
  delete client;
}

int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);
  int option;

  while ((option = getopt(argc, argv, "d:p:t:b:s:l:")) != -1) {
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
    case 'l':
      LOGFILE = string(optarg);
      break;
    default:
      cerr<< "usage: " << argv[0] << " [-p port] [-t threads] [-b buffers]" << endl;
      exit(1);
    }
  }

  set_log_file(LOGFILE);

  sync_print("init", "");
  MyServerSocket *server = new MyServerSocket(PORT);
  MySocket *client;

  // The order that you push services dictates the search order
  // for path prefix matching
  services.push_back(new AuthService());
  services.push_back(new TransferService());
  services.push_back(new DepositService());
  services.push_back(new AccountService());
  services.push_back(new FileService(BASEDIR));

  // Make sure that all services have a pointer to the
  // database object singleton
  Database *db = new Database();
  vector<HttpService *>::iterator iter;
  for (iter = services.begin(); iter != services.end(); iter++) {
    (*iter)->m_db = db;
  }

  // parse out config information
  stringstream config;
  int fd = open("config.json", O_RDONLY);
  if (fd < 0) {
    cout << "config.json not found" << endl;
    exit(1);
  }
  int ret;
  char buffer[4096];
  while ((ret = read(fd, buffer, sizeof(buffer))) > 0) {
    config << string(buffer, ret);
  }
  Document d;
  d.Parse(config.str());
  db->stripe_secret_key = d["stripe_secret_key"].GetString();
  
  while(true) {
    sync_print("waiting_to_accept", "");
    client = server->accept();
    sync_print("client_accepted", "");
    handle_request(client);
  }
}
