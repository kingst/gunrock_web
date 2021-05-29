#include "StringUtils.h"
#include "Base64.h"

#include <openssl/rand.h>

#define ERROR_RUNTIME_ERROR "error_runtime_error"
#define ERROR_NO_RANDOM_BYTES "error_no_random_bytes"

using namespace std;

string StringUtils::createUserId() {
  return createAuthToken();
}

string StringUtils::createAuthToken() {
  int numBytes = 18;
  unsigned char *buf = new unsigned char[numBytes];
  int rc = RAND_bytes(buf, numBytes);

  // might happen if entropy pool has been depleted
  if (rc != 1) {
    delete [] buf;
    throw ERROR_NO_RANDOM_BYTES;
  }

  string ret = "";
  try {
    ret = Base64::bytesToBase64UrlSafe(buf, numBytes);
  } catch (...) {
    delete [] buf;
    throw ERROR_RUNTIME_ERROR;
  }
  
  delete [] buf;
  return ret;
}

vector<string> StringUtils::splitWithDelimiter(string str, char delimiter) {
  vector<string> result;

  int start = 0;
  int idx;
  for (idx = 0; idx < (int) str.length(); idx++) {
    // if we're at the delimiter
    if (str[idx] == delimiter) {
      // if there is a string before add it
      if (idx != start) {
        result.push_back(str.substr(start, idx-start));
      }

      string s;
      s.push_back(delimiter);
      result.push_back(s);
      start = idx + 1;
    }
  }

  if (idx != start) {
    result.push_back(str.substr(start, idx-start));
  }

  return result;
}

vector<string> StringUtils::split(string str, char delimiter) {
  vector<string> result;
  vector<string> initial = splitWithDelimiter(str, delimiter);
  string d;
  d.push_back(delimiter);

  // filter out the delimiter tokens
  for (int idx = 0; idx < (int) initial.size(); idx++) {
    if (initial[idx] != d) {
      result.push_back(initial[idx]);
    }
  }

  return result;
}
