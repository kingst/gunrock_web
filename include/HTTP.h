#ifndef _HTTP_H_
#define _HTTP_H_

#include "http_parser.h"

#include <string>
#include <vector>
#include <map>

class HTTP {
 public:
    typedef enum {INIT, HEADER, FIELD, VALUE, BODY, DONE} HttpState;

    HTTP(http_parser_type httpType = HTTP_REQUEST);
    ~HTTP();

    int addData(const unsigned char *data, int len);
    bool isDone();
    bool isHeaderDone();
    std::string getProxyRequest(const char *userAgent = NULL);
    std::string getReplyHeader();
    std::string getHost();
    std::string getUrl();
    std::string getPath();
    bool isConnect() {return m_method == HTTP_CONNECT;}
    bool isHead() {return m_method == HTTP_HEAD;}
    bool isGet() {return m_method == HTTP_GET;}
    bool isPut() {return m_method == HTTP_PUT;}
    bool isPost() {return m_method == HTTP_POST;}
    bool isDelete() {return m_method == HTTP_DELETE;}
    std::string getBody();
    std::string getQuery() {return m_query;}
    std::vector< std::pair< std::string *, std::string *> > getHeaders() {
      return m_headers;
    }
  
 private:
    static int message_begin_cb(http_parser *parser);
    static int path_cb(http_parser *parser, const char *at, size_t length);
    static int query_string_cb(http_parser *parser, const char *at, size_t length);
    static int url_cb(http_parser *parser, const char *at, size_t length);
    static int fragment_cb(http_parser *parser, const char *at, size_t length);
    static int header_field_cb(http_parser *parser, const char *at, size_t length);
    static int header_value_cb(http_parser *parser, const char *at, size_t length);
    static int headers_complete_cb(http_parser *parser);
    static int body_cb(http_parser *parser, const char *at, size_t length);
    static int message_complete_cb(http_parser *parser);

    HttpState getState();
    void setState(HttpState newState);
    void appendUrl(const char *at, size_t len);
    void newHeaderField(const char *at, size_t len);
    void appendHeaderField(const char *at, size_t len);
    void appendHeaderValue(const char *at, size_t len);
    void addHeaderField();
    void messageComplete(unsigned char method);

    http_parser_settings m_settings;
    http_parser m_parser;
    HttpState m_state;
    bool m_doneParsing;
    bool m_headerDone;

    std::string m_url;
    std::string m_path;
    std::string m_query;
    std::string m_host;
    std::string *m_field;
    std::string *m_value;
    std::vector< std::pair< std::string *, std::string *> > m_headers;
    std::string m_body;
    std::string m_statusStr;
    unsigned char m_method;
    http_parser_type m_httpType;
    int m_extraParsedBytes;
};

#endif
