#include "HTTP.h"

#include <iostream>
#include <string>

#include <assert.h>
#include <stdio.h>

using namespace std;


/***************************** HTTP Parser callbacks ************************/

int HTTP::message_begin_cb(http_parser *parser)
{
    HTTP *http = (HTTP *) parser->data;
    assert(http->getState() == HTTP::INIT);
    http->setState(HTTP::HEADER);
    return 0;
}

int HTTP::path_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;
    http->m_path.append(at, length);
    return 0;
}
int HTTP::query_string_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;
    http->m_query.append(at, length);
    return 0;
}

int HTTP::url_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;
    http->appendUrl(at, length);

    return 0;
}

int HTTP::fragment_cb(http_parser */*parser*/, const char */*at*/, size_t /*length*/)
{
    cout << "fragment" << endl;
    assert(false);
    return 0;
}

int HTTP::header_field_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;

    if(http->getState() == HTTP::FIELD) {
        http->appendHeaderField(at, length);
    } else if((http->getState() == HTTP::VALUE) ||
              (http->getState() == HTTP::HEADER)) {
        http->newHeaderField(at, length);
        http->setState(HTTP::FIELD);
    } else {
        assert(false);
    }

    return 0;
}

int HTTP::header_value_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;
    if(http->getState() == HTTP::FIELD) {
        http->setState(HTTP::VALUE);
    }
    assert(http->getState() == HTTP::VALUE);
    http->appendHeaderValue(at, length);
    return 0;
}

int HTTP::headers_complete_cb(http_parser *parser)
{
    HTTP *http = (HTTP *) parser->data;
    http->addHeaderField();
    http->m_headerDone = true;

    if(http->m_httpType == HTTP_RESPONSE) {
        char buf[64];
        snprintf(buf, 63, "HTTP/%u.%u %u ", parser->http_major, parser->http_minor, parser->status_code);
        http->m_statusStr = buf;
        if(parser->status_code == 200) {
            http->m_statusStr += "OK";
        } else if(parser->status_code == 204) {
            http->m_statusStr += "No Content";
        } else if(parser->status_code == 301) {
            http->m_statusStr += "Moved Permanently";
        } else if(parser->status_code == 302) {
            http->m_statusStr += "Moved Temporarily";
        } else if(parser->status_code == 304) {
            http->m_statusStr += "Not Modified";
        } else if(parser->status_code == 403) {
            http->m_statusStr += "Forbidden";
        } else if(parser->status_code == 404) {
            http->m_statusStr += "Not Found";
        } else if(parser->status_code == 408) {
            http->m_statusStr += "Request Timeout";
        } else if(parser->status_code == 500) {
            http->m_statusStr += "Internal Server Error";
        } else if(parser->status_code == 503) {
            http->m_statusStr += "Service Unavailable";
        } else {
            assert(false);
        }
        
        http->m_extraParsedBytes = 1;
        return -1;
    }

    return 0;
}

int HTTP::body_cb(http_parser *parser, const char *at, size_t length)
{
    HTTP *http = (HTTP *) parser->data;
    http->m_body.append(at, length);

    return 0;
}

int HTTP::message_complete_cb(http_parser *parser)
{
    HTTP *http = (HTTP *) parser->data;
    assert((http->getState() == HTTP::VALUE) || 
           (http->getState() == HTTP::BODY));
    http->setState(HTTP::DONE);
    http->messageComplete(parser->method);
    return 0;
}

/****************************************************************************/



/*************************** Public Functions *******************************/


HTTP::HTTP(http_parser_type httpType)
{
    m_state = INIT;
    http_parser_init(&m_parser, httpType);
    m_doneParsing = false;
    m_httpType = httpType;
    m_headerDone = false;

    m_settings.on_message_begin = message_begin_cb;
    m_settings.on_path = path_cb;
    m_settings.on_query_string = query_string_cb;
    m_settings.on_url = url_cb;
    m_settings.on_fragment = fragment_cb;
    m_settings.on_header_field = header_field_cb;
    m_settings.on_header_value = header_value_cb;
    m_settings.on_headers_complete = headers_complete_cb;
    m_settings.on_body = body_cb;
    m_settings.on_message_complete = message_complete_cb;

    m_parser.data = this;

    m_field = NULL;
    m_value = NULL;
    m_extraParsedBytes = 0;
}

HTTP::~HTTP()
{
    if(m_field != NULL) {
        delete m_field;
    }

    if(m_value != NULL) {
        delete m_value;
    }

    for(unsigned int idx = 0; idx < m_headers.size(); idx++) {
        delete m_headers[idx].first;
        delete m_headers[idx].second;
    }
}

int HTTP::addData(const unsigned char *data, int len)
{
    if(m_doneParsing) {
        assert(false);
    }
    int ret = http_parser_execute(&m_parser, &m_settings, (const char *) data, len);
    ret += m_extraParsedBytes;
    m_extraParsedBytes = 0;
    return ret;
}

string HTTP::getBody()
{
    return m_body;
}

string HTTP::getUrl()
{
    return m_url;
}

string HTTP::getPath()
{
    return m_path;
}

string HTTP::getHost()
{
    string host = (m_method == HTTP_CONNECT) ? m_url : m_host;
    if(host.find(':') == string::npos) {
        host += ":80";
    }
    return host;
}

bool HTTP::isHeaderDone()
{
    return m_headerDone;
}

bool HTTP::isDone()
{
    return m_doneParsing;
}

string HTTP::getReplyHeader()
{
    string reply;

    assert(m_httpType == HTTP_RESPONSE);
    assert(m_statusStr.size() > 0);

    reply = m_statusStr + "\r\n";

    bool foundConn = false;
    for(unsigned int idx = 0; idx < m_headers.size(); idx++) {
        string field = *(m_headers[idx].first);
        string value = *(m_headers[idx].second);

        if(field == "Connection") {
            value = "close";
            foundConn = true;
        }

        reply += field + string(": ") + value + string("\r\n");
    }

    if(!foundConn) {
        reply += "Connection: close\r\n";
    }

    reply += "\r\n";

    return reply;
}

string HTTP::getProxyRequest(const char *userAgent)
{
    string reply;
    string urlPathQuery;

    assert(m_httpType == HTTP_REQUEST);

    if((m_method == HTTP_GET) || (m_method == HTTP_POST) || (m_method == HTTP_HEAD)) {
        if(m_path.size() == 0) {
            urlPathQuery = "/";
        } else {
            urlPathQuery = m_path;
        }
        if(m_query.size() > 0) {
            urlPathQuery += "?" + m_query;
        }
        if(m_url.find(urlPathQuery) == string::npos) {
            // this is a hack to get around buggy HTML from taobao
            assert(m_query.size() > 0);
            urlPathQuery = m_path + "??" + m_query;
            if(m_url.find(urlPathQuery) == string::npos) {
                cout << "url path mismatch " << m_url << endl << urlPathQuery << endl;
            }
        }
    }

    if(m_method == HTTP_GET) {
        reply = "GET " + urlPathQuery + " HTTP/1.1\r\n";
    } else if(m_method == HTTP_CONNECT) {
        reply = "CONNECT " + m_url + " HTTP/1.1\r\n";
    } else if(m_method == HTTP_POST) {
        reply = "POST " + urlPathQuery + " HTTP/1.1\r\n";
    } else if(m_method == HTTP_HEAD) {
        reply = "HEAD " + urlPathQuery + " HTTP/1.1\r\n";
    } else {
        assert(false);
    }

    for(unsigned int idx = 0; idx < m_headers.size(); idx++) {
        string field = *(m_headers[idx].first);
        string value = *(m_headers[idx].second);

        if((userAgent != NULL) && (field == "User-Agent")) {
            value = string(userAgent);
        }

        if(field == "Proxy-Connection") {
            field = string("Connection");
            value = string("close");
            //value = string("keep-alive");
        }

        if(field != "Keep-Alive") {
            reply += field + string(": ") + value + string("\r\n");
        }
    }

    reply += string("\r\n");
    if(m_body.size() > 0) {
        reply += m_body;
    }

    if(m_method == HTTP_HEAD) {
        cout << reply;
    }

    return reply;
}


/****************************************************************************/



/************************** Private Functions *******************************/

HTTP::HttpState HTTP::getState()
{
    return m_state;
}

void HTTP::setState(HttpState newState)
{
    m_state = newState;
}

void HTTP::appendUrl(const char *at, size_t len)
{
    m_url.append(at, len);
}

void HTTP::addHeaderField()
{
    if(m_field != NULL) {
        assert(m_value != NULL);
        if(*m_field == "Host") {
            m_host = *m_value;
        }
        if(*m_field == "Eoh") {
            cout << "got the Eoh header" << endl;
        }
        m_headers.insert(m_headers.end(), pair<string *, string *>(m_field, m_value));
        m_field = NULL;
        m_value = NULL;
    } else {
        assert(m_value == NULL);
    }
}

void HTTP::newHeaderField(const char *at, size_t len)
{
    addHeaderField();
    m_field = new string(at, len);
    m_value = new string();
}
void HTTP::appendHeaderField(const char *at, size_t len)
{
    assert(m_field != NULL);
    m_field->append(at, len);
}

void HTTP::appendHeaderValue(const char *at, size_t len)
{
    m_value->append(at, len);
}

void HTTP::messageComplete(unsigned char method)
{
    if(m_httpType == HTTP_REQUEST) {
      assert((method == HTTP_GET) || (method == HTTP_CONNECT) || (method == HTTP_POST) || (method == HTTP_HEAD) || (method == HTTP_PUT) || (method == HTTP_DELETE) || (method == HTTP_MOVE));
        m_method = method;
    }
    m_doneParsing = true;
}

/****************************************************************************/
