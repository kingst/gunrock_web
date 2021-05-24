#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdint.h>
#include <string>

class Base64 {
public:
  static std::string bytesToBase64(const uint8_t *data, int len);
  static std::string bytesToBase64UrlSafe(const uint8_t *data, int len);
  static uint8_t *base64ToBytes(std::string s, int *len);
};

#endif
