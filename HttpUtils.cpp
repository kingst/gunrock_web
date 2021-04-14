#include <assert.h>

#include "HttpUtils.h"

using namespace std;

map<string, string> HttpUtils::params(string query) {
  map<string, string> paramMap;

  if (query.size() == 0) {
    return paramMap;
  }

  vector<string> pairs = split(query, '&');
  for (unsigned idx = 0; idx < pairs.size(); idx++) {
    string param = pairs[idx];
    vector<string> elements = split(param, '=');
    if (elements.size() != 2) {
      throw MalformedQueryString(query);
    }

    // XXX FIXME we need to url decode the strings
    paramMap[elements[0]] = elements[1];
  }

  return paramMap;
}

void HttpUtils::writeChunk(MySocket *client,
				      const void *buf, int numBytes) {

  char chunkHeader[256];
  snprintf(chunkHeader, sizeof(chunkHeader), "%x\r\n", numBytes);
  client->write(chunkHeader);
  if (buf != NULL && numBytes > 0) {
    client->write(string((const char *) buf, numBytes));
  }
  client->write("\r\n");
}

void HttpUtils::writeLastChunk(MySocket *client) {
  writeChunk(client, NULL, 0);
}


// split lifted from stackoverflow
// http://stackoverflow.com/questions/236129/split-a-string-in-c
vector<string> &HttpUtils::split(const string &s,
		      char delim,
		      vector<string> &elems) {

  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

vector<string> HttpUtils::split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, elems);

  vector<string> result;
  for (unsigned int idx = 0; idx < elems.size(); idx++) {
    if (elems[idx].size() > 0) {
      result.push_back(elems[idx]);
    }
  }
  return result;
}
