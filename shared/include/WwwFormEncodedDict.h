#ifndef _WWW_FORM_ENCODED_DICT_H_
#define _WWW_FORM_ENCODED_DICT_H_

#include <string>
#include <map>
#include <vector>

class WwwFormEncodedDict {
 public:
  WwwFormEncodedDict();
  WwwFormEncodedDict(std::string body);
  
  std::string get(std::string key);
  void set(std::string key, std::string value);
  void set(std::string key, int value);
  
  std::string encode();

 protected:
  std::string urlencode(std::string str);
  std::string urldecode(std::string str);
  std::vector<std::string> split(std::string str, char delimiter);
  std::vector<std::string> splitWithDelimiter(std::string str, char delimiter);
  
 private:
  std::map<std::string, std::string> m_values;
};
  
#endif
