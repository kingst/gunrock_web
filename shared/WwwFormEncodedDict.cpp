#include "WwwFormEncodedDict.h"
#include "StringUtils.h"

#include <sstream>

#include <stdio.h>
#include <ctype.h>

using namespace std;

WwwFormEncodedDict::WwwFormEncodedDict() {
  // nothing needed
}

WwwFormEncodedDict::WwwFormEncodedDict(string body) {
  //parse the body
  vector<string> pairs = StringUtils::split(body, '&');
  vector<string>::iterator iter;
  for (iter = pairs.begin(); iter != pairs.end(); iter++) {
    vector<string> keyValue = StringUtils::split(*iter, '=');
    if (keyValue.size() != 2) {
      throw "Parse error";
    }

    string key = urldecode(keyValue[0]);
    string value = urldecode(keyValue[1]);
    m_values[key] = value;
  }
}

string WwwFormEncodedDict::get(string key) {
  return m_values[key];
}

void WwwFormEncodedDict::set(string key, string value) {
  m_values[key] = value;
}

void WwwFormEncodedDict::set(string key, int value) {
  stringstream stream;
  stream << value;
  m_values[key] = stream.str();
}

string WwwFormEncodedDict::encode() {
  stringstream output;
  map<string, string>::iterator iterator;
  for (iterator = m_values.begin(); iterator != m_values.end(); iterator++) {
    if (iterator != m_values.begin()) {
      output << "&";
    }
    output << urlencode(iterator->first) << "=" << urlencode(iterator->second);
  }

  return output.str();
}

string WwwFormEncodedDict::urlencode(std::string str) {
  string::iterator iterator;
  stringstream encoded;
  for (iterator = str.begin(); iterator != str.end(); iterator++) {
    char c = *iterator;
    if (isalnum(c)) {
      encoded << c;
    } else {
      char hex[5];
      snprintf(hex, sizeof(hex), "%%%02x", c);
      encoded << hex;
    }
  }

  return encoded.str();
}

string WwwFormEncodedDict::urldecode(std::string str) {
  vector<string> lines = StringUtils::splitWithDelimiter(str, '%');
  bool nextStartsWithHex = false;
  stringstream decoded;
  
  vector<string>::iterator iter;
  for (iter = lines.begin(); iter != lines.end(); iter++) {
    string line = *iter;
    if (line == "%") {
      nextStartsWithHex = true;
    } else if (nextStartsWithHex) {
      string hex = line.substr(0, 2);
      unsigned int digit;
      int ret = sscanf(hex.c_str(), "%02x", &digit);
      if (ret != 1) {
	throw "couldn't parse escaped char";
      }
      decoded << (char) digit;
      decoded << line.substr(2);
      nextStartsWithHex = false;
    } else {
      decoded << line;
    }
  }

  return decoded.str();
}
