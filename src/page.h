#ifndef _PAGE_H_
#define _PAGE_H_

#include "config.h"
#include <string.h>

_START_MYJFM_NAMESPACE_

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

class Page {
};

_END_MYJFM_NAMESPACE_

#endif

