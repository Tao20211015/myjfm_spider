/*******************************************************************************
 * page.h - the HttpResponseHeader and Page class implementation
 * The HttpResponseHeader class is used for recording the http 
 * response information. If and only if the status_code is 200, 
 * the response is valid
 * The Page class uses memory pool to get memory to cache the page content.
 * Downloaders put the page content into it, and then transfer it to the
 * extractors. And, extractors extract urls from it, then save the page onto
 * disk, then transfer the urls to schedulers
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _PAGE_H_
#define _PAGE_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "shared.h"
#include "utility.h"
#include "memory_pool.h"

_START_MYJFM_NAMESPACE_

// Http response header class
// This class is used for recording the http response information
// if and only if the status_code is 200, the response is valid
class HttpResponseHeader {
public:
  enum http_encoding_type {
    HTTP_ENCODING_UNKNOWN = -1, 
    HTTP_ENCODING_TXT = 0, 
    HTTP_ENCODING_GZIP = 1, 
    HTTP_ENCODING_DEFLATE = 2 
  };

  HttpResponseHeader() : 
    _is_valid(false), 
    _http_version(1), 
    _status_code(0), 
    _content_type(""), 
    _charset(""), 
    _content_length(-1), 
    _content_encoding(HTTP_ENCODING_TXT), 
    _connection(true), 
    _location("") {
  }

  inline bool is_valid() {
    return _is_valid;
  }

  inline RES_CODE get_http_version(int32_t& http_version) {
    if (_is_valid) {
      http_version = _http_version;
      return S_OK;
    }

    http_version = -1;
    return S_FAIL;
  }

  inline RES_CODE get_status_code(int32_t& status_code) {
    if (_is_valid) {
      status_code = _status_code;
      return S_OK;
    }

    status_code = -1;
    return S_FAIL;
  }

  inline RES_CODE get_content_type(String& content_type) {
    if (_is_valid) {
      content_type = _content_type;
      return S_OK;
    }

    content_type = "";
    return S_FAIL;
  }

  inline RES_CODE get_charset(String& charset) {
    if (_is_valid) {
      charset = _charset;
      return S_OK;
    }

    charset = "";
    return S_FAIL;
  }

  inline RES_CODE get_content_length(int32_t& content_length) {
    if (_is_valid) {
      content_length = _content_length;
      return S_OK;
    }

    content_length = -1;
    return S_FAIL;
  }

  inline RES_CODE get_content_encoding(http_encoding_type& content_encoding) {
    if (_is_valid) {
      content_encoding = _content_encoding;
      return S_OK;
    }

    content_encoding = HTTP_ENCODING_UNKNOWN;
    return S_FAIL;
  }

  inline RES_CODE get_connection(bool& connection) {
    if (_is_valid) {
      connection = _connection;
      return S_OK;
    }

    connection = false;
    return S_FAIL;
  }

  inline RES_CODE get_location(String& location) {
    if (_is_valid) {
      location = _location;
      return S_OK;
    }

    location = "";
    return S_FAIL;
  }

  inline RES_CODE analysis(char* header) {
    if (!header) {
      _is_valid = false;
      return S_FAIL;
    }

    int32_t header_len = strlen(header);
    if (header_len < 16) { // is not HTTP/1.1 200 xx
      _is_valid = false;
      return S_FAIL;
    }

    header[8] = '\0';

    if (!strcmp(header, "HTTP/1.1")) {
      _http_version = 1;
    } else if (!strcmp(header, "HTTP/1.0")) {
      _http_version = 0;
    } else {
      _is_valid = false;
      return S_FAIL;
    }

    header += 9;
    header_len -= 9;

    if (isdigit(header[0]) && 
        isdigit(header[1]) && 
        isdigit(header[2]) && 
        header[3] == ' ') {
      _status_code = 100 * header[0] + 10 * header[1] + header[2] - 111 * '0';
    } else {
      _is_valid = false;
      return S_FAIL;
    }

    int32_t i = 0;
    while (header[i] != '\0' && header[i] != '\n') {
      i++;
    }

    if (header[i] == '\0') {
      _is_valid = false;
      return S_FAIL;
    }

    header += i + 1;
    header_len -= i + 1;

    while (header_len > 0) {
      i = 0;
      while (header[i] != '\0' && header[i] != '\n') {
        i++;
      }
      
      header[i] = '\0';
      int32_t j;
      for (j = 0; j < i; ++j) {
        if (header[j] == ':') {
          break;
        }
      }

      if (j < i - 1) { // in case of "xxxxx: \0"
        header[j] = '\0';
        char* key = header;
        char* value = header + j + 2;
        Utility::tolower(key);
        Utility::tolower(value);
        if (!strcmp(key, "content-length")) {
          uint32_t length = 0;
          if (Utility::str2integer(value, length) == S_OK) {
            _content_length = length;
          }
        } else if (!strcmp(key, "content-type")) {
          _content_type = value;
          Vector<String> items;
          Utility::split(_content_type, CHARS2STR("; "), items);
          if (items.size() == 0) {
            _content_type = "";
            _charset = "";
          } else if (items.size() == 1) {
            _content_type = items[0];
            _charset = "";
          } else if (items.size() == 2) {
            _content_type = items[0];
            _charset = items[1];
            Utility::split(_charset, CHARS2STR("="), items);
            if (items.size() != 2 || items[0] != CHARS2STR("charset")) {
              _charset = "";
            } else {
              _charset = items[1];
            }
          } else {
            _content_type = items[0];
            _charset = "";
          }
        } else if (!strcmp(key, "content-encoding")) {
          if (!strcmp(value, "gzip")) {
            _content_encoding = HTTP_ENCODING_GZIP;
          } else if (!strcmp(value, "deflate")) {
            _content_encoding = HTTP_ENCODING_DEFLATE;
          } else {
            _content_encoding = HTTP_ENCODING_UNKNOWN;
          }
        } else if (!strcmp(key, "transfer-encoding")) {
          if (!strcmp(value, "chunked")) {
            _content_length = -1;
          } else { // only support the chunked transfer-coding
            _is_valid = false;
            return S_FAIL;
          }
        } else if (!strcmp(key, "location")) {
          _location = value;
        } else if (!strcmp(key, "connection")) {
          if (!strcmp(value, "close")) {
            _connection = false;
          }
        }

        header += i + 1;
        header_len -= i + 1;
      }
    }

    _is_valid = true;
    return S_OK;
  }

private:
  bool _is_valid;
  int32_t _http_version;
  int32_t _status_code;
  String _content_type;
  String _charset;
  int32_t _content_length;
  http_encoding_type _content_encoding;
  bool _connection;
  String _location;
};

// This is the page class
// It use memory pool to get memory to cache the page content
// downloaders put the page content into it, and then transfer it to the
// extractors, and, extractors extract urls from it, then save the page onto
// disk, then transfer the urls to schedulers
class Page : public Shared {
public:
  Page(uint32_t length = 0) {
    if (length == 0) {
      _page_content = NULL;
      _page_size = 0;
    } else {
      MemoryPool* memory_pool = MemoryPool::get_instance();
      if (memory_pool) {
       _page_content = (char*)(memory_pool->get_memory(length));
       if (_page_content) {
         _page_size = length;
       } else {
         _page_size = 0;
       }
      } else {
        _page_content = (char*)malloc(length);
        if (_page_content) {
          _page_size = length;
        } else {
          _page_size = 0;
        }
      }
    }
  }

  ~Page() {
    if (_page_content) {
      MemoryPool* memory_pool = MemoryPool::get_instance();
      if (memory_pool) {
        memory_pool->put_memory(_page_content);
      } else {
        free(_page_content);
      }
    }
  }

  RES_CODE get_page_size(uint32_t& page_size) {
    page_size = _page_size;

    return S_OK;
  }

  RES_CODE get_page_content(char*& page_content) {
    page_content = _page_content;

    return S_OK;
  }

  RES_CODE get_site(String& site) {
    site = _site;

    return S_OK;
  }

  RES_CODE set_site(String& site) {
    _site = site;

    return S_OK;
  }

  RES_CODE get_port(uint16_t& port) {
    port = _port;

    return S_OK;
  }

  RES_CODE set_port(uint16_t& port) {
    _port = port;

    return S_OK;
  }

  RES_CODE get_file(String& file) {
    file = _file;

    return S_OK;
  }

  RES_CODE set_file(String& file) {
    _file = file;

    return S_OK;
  }

  RES_CODE get_charset(String& charset) {
    charset = _charset;

    return S_OK;
  }

  RES_CODE set_charset(String& charset) {
    _charset = charset;

    return S_OK;
  }

  RES_CODE get_encoding(HttpResponseHeader::http_encoding_type& encoding) {
    encoding = _encoding;

    return S_OK;
  }

  RES_CODE set_encoding(HttpResponseHeader::http_encoding_type& encoding) {
    _encoding = encoding;

    return S_OK;
  }

private:
  char* _page_content;
  uint32_t _page_size;
  String _site;
  uint16_t _port;
  String _file;
  String _charset;
  HttpResponseHeader::http_encoding_type _encoding;
};

_END_MYJFM_NAMESPACE_

#endif

