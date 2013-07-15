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

#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "memory_pool.h"

_START_MYJFM_NAMESPACE_

// Http response header class
// This class is used for recording the http response information
// if and only if the status_code is 200, the response is valid
class HttpResponseHeader {
public:
  HttpResponseHeader() : 
    _is_valid(0), 
    _http_version(1), 
    _status_code(0), 
    _content_type(""), 
    _content_length(-1) {
  }

  inline int is_valid() {
    return _is_valid;
  }

  inline RES_CODE get_http_version(int& http_version) {
    if (_is_valid) {
      http_version = _http_version;
      return S_OK;
    }

    http_version = -1;
    return S_FAIL;
  }

  inline RES_CODE get_status_code(int& status_code) {
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

  inline RES_CODE get_content_length(int& content_length) {
    if (_is_valid) {
      content_length = _content_length;
      return S_OK;
    }

    content_length = -1;
    return S_FAIL;
  }

  inline RES_CODE analysis(char* header) {
    if (!header) {
      _is_valid = 0;
      return S_FAIL;
    }

    int header_len = strlen(header);
    if (header_len < 16) { // is not HTTP/1.1 200 xx
      _is_valid = 0;
      return S_FAIL;
    }

    header[8] = '\0';

    if (!strcmp(header, "HTTP/1.1")) {
      _http_version = 1;
    } else if (!strcmp(header, "HTTP/1.0")) {
      _http_version = 0;
    } else {
      _is_valid = 0;
      return S_FAIL;
    }

    header = header + 9;
    header_len -= 9;

    if (isdigit(header[0]) && 
        isdigit(header[1]) && 
        isdigit(header[2]) && 
        header[3] == ' ') {
      _status_code = 100 * header[0] + 10 * header[1] + header[2] - 111 * '0';
    } else {
      _is_valid = 0;
      return S_FAIL;
    }

    int i = 0;
    while (header[i] != '\0' && header[i] != '\n') {
      i++;
    }

    if (header[i] == '\0') {
      _is_valid = 0;
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
      int j;
      for (j = 0; j < i; ++j) {
        if (header[j] == ':') {
          break;
        }
      }

      if (j < i - 1) { // in case of "xxxxx: \0"
        header[j] = '\0';
        char* key = header;
        char* value = header + j + 2;
        if (!strcmp(key, "Content-Length")) {
          int length = -1;
          if (Utility::str2integer(value, length) == S_OK) {
            _content_length = length;
          }
        } else if (!strcmp(key, "Content-Type")) {
          _content_type = value;
        } else if (!strcmp(key, "Transfer-Encoding")) {
          _content_length = -1;
        }

        header += i + 1;
        header_len -= i + 1;
      }
    }

    _is_valid = 1;
    return S_OK;
  }

private:
  int _is_valid;
  int _http_version;
  int _status_code;
  String _content_type;
  int _content_length;
};

// This is the page class
// It use memory pool to get memory to cache the page content
// downloaders put the page content into it, and then transfer it to the
// extractors, and, extractors extract urls from it, then save the page onto
// disk, then transfer the urls to schedulers
class Page : public Shared {
public:
  Page(int length = 0) {
    if (length <= 0) {
      _page_content = NULL;
      _page_size = 0;
    } else {
      MemoryPool* memory_pool = MemoryPool::get_instance();
      if (memory_pool) {
       _page_content = (char*)(memory_pool->get_memory(length));
       if (_page_content) {
         _page_size = length;
       } else {
         _page_size = -1;
       }
      } else {
        _page_content = (char*)malloc(length);
        if (_page_content) {
          _page_size = length;
        } else {
          _page_size = -1;
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

  RES_CODE get_page_size(int& page_size) {
    page_size = _page_size;

    return S_OK;
  }

  RES_CODE get_page_content(char*& page_content) {
    page_content = _page_content;

    return S_OK;
  }

private:
  char* _page_content;
  int _page_size;
};

_END_MYJFM_NAMESPACE_

#endif

