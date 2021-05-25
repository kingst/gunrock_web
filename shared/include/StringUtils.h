#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <string>
#include <vector>

class StringUtils {
 public:
  static std::vector<std::string> splitWithDelimiter(std::string str, char delimiter);
  static std::vector<std::string> split(std::string str, char delimiter);
  static std::string createAuthToken();
  static std::string createUserId();
};

#endif
