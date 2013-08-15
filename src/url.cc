/*******************************************************************************
 * url.cc - the url class
 * it includes the protocol(HTTP), the site's name, the port, the requested file
 * name, etc.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>

#include "config.h"
#include "url.h"
#include "utility.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

Url::Url(const String& url) : 
  _raw_url(""), 
  _status(UNINITIALIZED), 
  _protocol(PRO_DUMMY), 
  _port(0), 
  _site(""), 
  _file(""), 
  has_get_md5(false), 
  _retries(0), 
  _has_dnsed_cname(false) {
  _ips.clear();

  if (url.length() == 0) {
    return;
  }

  if (url.length() <= 7) {
    return;
  }

  // only support HTTP protocol for now
  if (url.compare(0, 7, "http://") != 0) {
    return;
  }

  _protocol = HTTP;

  String_size_t site_pos = -1;
  String_size_t port_pos = -1;

  site_pos = url.find("/", 7);
  if (site_pos == String_tail) { // no requested file
    _file = "/";
    port_pos = url.find(":", 7);
    if (port_pos == String_tail) { // not specify port
      _site = url.substr(7);
      _port = 80; // default port is 80
    } else {
      _site = url.substr(7, port_pos - 7);
      if (Utility::str2integer((url.substr(port_pos + 1)).c_str(), 
            _port) != S_OK) {
        _site = "";
        _port = 0;
        _protocol = PRO_DUMMY;
        return;
      }
    }
  } else { // have file parameter in the url
    port_pos = url.find(":", 7);
    if (port_pos == String_tail || port_pos > site_pos) { // not specify port
      _site = url.substr(7, site_pos - 7);
      _port = 80;
    } else {
      _site = url.substr(7, port_pos - 7);
      if (Utility::str2integer(
            (url.substr(port_pos + 1, site_pos - port_pos - 1)).c_str(), 
            _port) != S_OK) {
        _site = "";
        _port = 0;
        _protocol = PRO_DUMMY;
        return;
      }
    }

    _file = url.substr(site_pos);
  }

  _status = INITIALIZED;
  _raw_url = url;
  return;
}

RES_CODE Url::get_status(EnumStatus& status) {
  status = _status;

  return S_OK;
}

RES_CODE Url::get_protocol(Protocol& protocol) {
  if (_status == UNINITIALIZED) {
    return S_FAIL;
  }

  protocol = _protocol;

  return S_OK;
}

RES_CODE Url::set_ip(Vector<uint32_t>& ips) {
  if (_status == UNINITIALIZED) {
    return S_FAIL;
  }

  _ips = ips;
  _status = DNSED;

  return S_OK;
}

RES_CODE Url::get_ip(Vector<uint32_t>& ips) {
  if (_status != DNSED) {
    return S_FAIL;
  }

  ips = _ips;

  return S_OK;
}

RES_CODE Url::get_port(uint16_t& port) {
  if (_status == UNINITIALIZED) {
    return S_FAIL;
  }

  port = _port;

  return S_OK;
}

RES_CODE Url::get_site(String& site) {
  if (_status == UNINITIALIZED) {
    return S_FAIL;
  }

  site = _site;

  return S_OK;
}

RES_CODE Url::get_file(String& file) {
  if (_status == UNINITIALIZED) {
    return S_FAIL;
  }

  file = _file;

  return S_OK;
}

RES_CODE Url::get_md5(MD5& md5) {
  if (_status == UNINITIALIZED) {
    md5 = MD5();
    return S_FAIL;
  }

  if (has_get_md5) {
    md5 = _md5;
    return S_OK;
  }

  String url = "";

  if (_protocol == HTTP) {
    url += "http://";
  } else {
    md5 = MD5();
    return S_FAIL;
  }

  url += _site;

  if (_port > 0 && _port != 80) {
    String str_port;
    Utility::integer2str(_port, str_port);
    url += ":" + str_port;
  }
  
  url += _file;

  MD5Caculator md5caculator(url);
  md5caculator.digest(_md5);
  md5 = _md5;
  has_get_md5 = true;

  return S_OK;
}

RES_CODE Url::get_retries(uint32_t& retries) {
  retries = _retries;

  return S_OK;
}

RES_CODE Url::inc_retries() {
  ++_retries;

  return S_OK;
}

bool Url::get_has_dnsed_cname() {
  return _has_dnsed_cname;
}

RES_CODE Url::set_has_dnsed_cname(bool has_dnsed_cname) {
  _has_dnsed_cname = has_dnsed_cname;

  return S_OK;
}

_END_MYJFM_NAMESPACE_

