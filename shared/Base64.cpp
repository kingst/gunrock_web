/**
 * Copyright 2013 Adrenaline Mobility.  All rights reserved.
 *
 * See the AUTHORS and LICENSE files for additional information on
 * contributors and the software license agreement.
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <string>
#include <iostream>

#include "Base64.h"

#define ERROR_INVALID_BASE64_CHAR "error_invalid_base64_char"
#define ERROR_INVALID_BASE64_STRING_LENGTH "error_invalid_base64_string_length"

using namespace std;

/**
 * Convert 6 bit values to base64 chars
 */
static char getBase64CharValue(uint8_t bits) {
        // Note: the bits >= 0 check is omitted because these are
        // unsigned numbers and the compiler is complaining
        if (bits <= 25) {
                return 'A' + bits;
        }

        if ((bits >= 26) && (bits <= 51)) {
                return 'a' + bits - 26;
        }

        if ((bits >= 52) && (bits <= 61)) {
                return '0' + bits - 52;
        }

        if (bits == 62) {
                return '+';
        }

        if (bits == 63) {
                return '/';
        }

        // sanity check, should be impossible to trigger with public
        // functions
        throw "invalid Base64 bit value";
}

/**
 * Convert base64 char to a 6 bit value
 */
static uint8_t getBase64ByteValue(char c) {
        if ((c >= 'A') && (c <= 'Z')) {
                return c - 'A';
        }

        if ((c >= 'a') && (c <= 'z')) {
                return c - 'a' + 26;
        }

        if ((c >= '0') && (c <= '9')) {
                return c - '0' + 52;
        }

        if (c == '+') {
                return 62;
        }

        if (c == '/') {
                return 63;
        }

        if (c == '=') {
                return 0;
        }

        throw ERROR_INVALID_BASE64_CHAR;
}

// this function operates on 4 char strings
static void decodeBase64Chunk(const char *str, uint8_t *bytes, bool isLastChunk) {
        int numBytes = 3;

        // Error checking and size adjustment for = char.
        if (str[0] == '=' || str[1] == '=') {
                throw ERROR_INVALID_BASE64_CHAR;
        }

        // = only allowed in last chunk
        if (isLastChunk) {
                if (str[3] == '=') {
                        if (str[2] == '=') {
                                numBytes = 1;
                        } else {
                                numBytes = 2;
                        }
                } else if (str[2] == '=') {
                        throw ERROR_INVALID_BASE64_CHAR;
                }
        } else {
                if (str[2] == '=' || str[3] == '=') {
                        throw ERROR_INVALID_BASE64_CHAR;
                }
        }

        uint32_t bits = 0;
        bits = getBase64ByteValue(str[0]) << 18 |
                getBase64ByteValue(str[1]) << 12 |
                getBase64ByteValue(str[2]) << 6 |
                getBase64ByteValue(str[3]);

        bytes[0] = (uint8_t) ((bits & 0x00ff0000) >> 16);
        if (numBytes >= 2) {
                bytes[1] = (uint8_t) ((bits & 0x0000ff00) >> 8);
        }
        if (numBytes == 3) {
                bytes[2] = (uint8_t) (bits & 0x000000ff);
        }
}

static void encodeBase64Chunk(const uint8_t *data, int len, string *ret) {
        char chunk[5];
        chunk[4] = '\0';
        unsigned long bits = 0;

        if (len <= 0) {
                // sanity check, should be impossible to trigger with
                // public functions
                throw "invalid chunk size";
        }

        if (len > 3) {
                len = 3;
        }

        bits |= data[0] << 16;
        if (len > 1) {
                bits |= data[1] << 8;
        }
        if (len > 2) {
                bits |= data[2];
        }

        chunk[4] = '\0';
        chunk[0] = getBase64CharValue((bits & (0x3f << 18)) >> 18);
        chunk[1] = getBase64CharValue((bits & (0x3f << 12)) >> 12);
        if (len == 1) {
                chunk[2] = '=';
                chunk[3] = '=';
        } else {
                chunk[2] = getBase64CharValue((bits & (0x3f << 6)) >> 6);
                if (len == 2) {
                        chunk[3] = '=';
                } else {
                        chunk[3] = getBase64CharValue(bits & 0x3f);
                }
        }

        *ret += chunk;
}

/**
 * Converts len bytes starting at data into a base64 encoded string,
 * which it returns.
 */
string Base64::bytesToBase64(const uint8_t *data, int len) {
        string ret;

        ret.reserve(static_cast<int>((len + 3) * 4 / 3));
        for (int idx = 0; idx < len; idx += 3) {
                encodeBase64Chunk(data + idx, len - idx, &ret);
        }

        return ret;
}

string replace_all(string str, string original, string replace) {
  string::size_type n = 0;
  while ( (n = str.find(original, n) ) != string::npos ) {
    str.replace(n, original.size(), replace);
    n += replace.size();
  }
  return str;
}

/**
 * Same as `bytesToBase64 but makes them URL safe
 */
string Base64::bytesToBase64UrlSafe(const uint8_t *data, int len) {
  string ret = bytesToBase64(data, len);
  ret = replace_all(ret, "+", "-");
  ret = replace_all(ret, "/", "_");
  return ret;
}

/**
 * Converts the string s from a base64 string into a uint8_t array of
 * bytes.  Stores the length of the byte array returned in the len
 * argument.
 */
uint8_t *Base64::base64ToBytes(string s, int *len) {
        if (s.size() == 0) {
                return NULL;
        }

        // figure out the number of bytes encoded
        int rem = s.size() % 4;
        int size = (s.size() / 4) * 3;
        if (rem == 0) {
                // check for padding
                if (s.at(s.size()-1) == '=') {
                        if (s.at(s.size()-2) == '=') {
                                size -= 2;
                        } else {
                                size -= 1;
                        }
                }
        } else {
                // no padding, not divisible by 4, add padding and
                // adjust size
                if (rem == 3) {
                        size += 2;
                        s += "=";
                } else if (rem == 2) {
                        size += 1;
                        s += "==";
                } else {
                        throw ERROR_INVALID_BASE64_STRING_LENGTH;
                }
        }

        *len = size;
        uint8_t *bytes = NULL;

        try {
                bytes = new uint8_t[size];
                unsigned int byteIdx = 0;
                for (unsigned int idx = 0; idx < s.size(); idx += 4) {
                        bool isLastChunk = (idx == (s.size() - 4));
                        decodeBase64Chunk(s.c_str() + idx, bytes + byteIdx,
                                          isLastChunk);
                        byteIdx += 3;
                }
        } catch (const char * ex) {
                if (bytes != NULL)
                        delete [] bytes;
                throw ex;
        }

        return bytes;
}
