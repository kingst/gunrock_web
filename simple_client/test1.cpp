#include <iostream>

#include <assert.h>

#include "HttpClient.h"

using namespace std;

int PORT = 8080;

int main(int argc, char *argv[]) {
  cout << "Making http request" << endl;
  
  HttpClient client("localhost", PORT);
  HTTPResponse *response = client.get("/hello_world.html");
  assert(response->status() == 200);
  delete response;
  
  cout << "passed" << endl;
  
  return 0;
}
