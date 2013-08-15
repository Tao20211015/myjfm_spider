#ifndef _URI_H_
#define _URI_H_

#include <stdio.h>

#define AT_SERVER 0
#define AT_REG_NAME 1

#define C_SCHEME 0001
#define C_AUTHORITY 0036
#define C_USERINFO 0002
#define C_HOST 0004
#define C_PORT 0010
#define C_REG_NAME 0020
#define C_PATH 0040
#define C_QUERY 0100
#define C_FRAGMENT 0200
#define C_URI 0377

#define FREE_ARRAY_IF_NOT_NULL(p) \
  do { \
    if (p) { \
      delete [] (p); \
      p = NULL; \
    } \
  } while (0)

_START_MYJFM_NAMESPACE_

class Authority {
public:
  int _type;
  union {
    struct {
      char* _userinfo;
      char* _host;
			char* _port;
    };
    char* _reg_name;
	};

  Authority(int type) {
    reinit(type);
  }

  ~Authority() {
    destroy();
  }

  Authority* get_copy() {
    Authority* res = new Authority(_type);
    if (_type == AT_SERVER) {
      res->_userinfo = Utility::strdup(_userinfo);
      res->_host = Utility::strdup(_host);
      res->_port = Utility::strdup(_port);
    } else {
      res->_reg_name = Utility::strdup(_reg_name);
    }

    return res;
  }

  void reinit(int type) {
    _type = type;
    if (_type == AT_SERVER) {
      _userinfo = NULL;
      _host = NULL;
      _port = NULL;
    } else {
      _reg_name = NULL;
    }
  }

  void destroy() {
    if (_type == AT_SERVER) {
      FREE_ARRAY_IF_NOT_NULL(_userinfo);
      FREE_ARRAY_IF_NOT_NULL(_host);
      FREE_ARRAY_IF_NOT_NULL(_port);
    } else {
      FREE_ARRAY_IF_NOT_NULL(_reg_name);
    }
    _type = -1;
  }
};

class Uri {
public:
  static char _uri_chr[16];

  char* _scheme;
  char* _path;
  Authority* _authority;
  char* _query;
  char* _fragment;

  Uri() : _scheme(NULL), _path(NULL), 
  _authority(NULL), _query(NULL), _fragment(NULL) {}
  ~Uri() {
    reinit();
  }

  void reinit() {
    FREE_ARRAY_IF_NOT_NULL(_scheme);
    FREE_ARRAY_IF_NOT_NULL(_path);
    if (_authority) {
      _authority->destroy();
      delete _authority;
      _authority = NULL;
    }
    FREE_ARRAY_IF_NOT_NULL(_query);
    FREE_ARRAY_IF_NOT_NULL(_fragment);
  }

  RES_CODE copy_from(Uri& uri) {
    reinit();
    _scheme = Utility::strdup(uri._scheme);
    _path = Utility::strdup(uri._path);
    _authority = (uri._authority)->get_copy();
    _query = Utility::strdup(uri._query);
    _fragment = Utility::strdup(uri._fragment);
    return S_OK;
  }
};

_END_MYJFM_NAMESPACE_

#endif

