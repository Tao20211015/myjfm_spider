/*******************************************************************************
 * url.cc - the url class
 * it includes the protocol(HTTP), the site's name, the port, the requested file
 * name, etc.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include "config.h"
#include "url.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

Url::Url(const String& url) : 
  _raw_url(""), 
  _is_valid(0), 
  _protocol(PRO_DUMMY), 
  _site(""), 
  _port(""), 
  _file(""), 
  has_get_md5(0), 
  _retries(0) {
  if (url.length() <= 0) {
    return;
  }

  String_size_t site_pos = -1;
  String_size_t port_pos = -1;

  if (url.length() <= 7) {
    return;
  }

  // only support HTTP protocol for now
  if (url.compare(0, 7, "http://") != 0) {
    return;
  }

  _protocol = HTTP;

  String no_proto_url = url.substr(7);

  site_pos = no_proto_url.find("/");
  if (site_pos == String_tail) { // no requested file
    port_pos = no_proto_url.find(":");
    if (port_pos == String_tail) { // not specify port
      _site = no_proto_url;
      _port = "80"; // default port is 80
    } else {
      _site = no_proto_url.substr(0, port_pos);
      _port = no_proto_url.substr(port_pos + 1);
    }
  } else { // have file parameter in the url
    port_pos = no_proto_url.find(":");
    if (port_pos == String_tail || port_pos > site_pos) { // not specify port
      _site = no_proto_url.substr(0, site_pos);
      _port = "80";
    } else {
      _site = no_proto_url.substr(0, port_pos);
      _port = no_proto_url.substr(port_pos + 1, site_pos - port_pos - 1);
    }

    _file = no_proto_url.substr(site_pos + 1);
  }

  _is_valid = 1;
  _raw_url = url;
  return;
}

RES_CODE Url::is_valid(int& valid) {
  valid = _is_valid;
  return S_OK;
}

RES_CODE Url::get_protocol(Protocol& protocol) {
  protocol = _protocol;

  return S_OK;
}

RES_CODE Url::get_site(String& site) {
  site = _site;

  return S_OK;
}

RES_CODE Url::get_port(String& port) {
  port = _port;

  return S_OK;
}

RES_CODE Url::get_file(String& file) {
  file = _file;

  return S_OK;
}

RES_CODE Url::get_md5(MD5& md5) {
  if (has_get_md5) {
    md5 = _md5;
    return S_OK;
  }

  if (!_is_valid || _protocol == PRO_DUMMY) {
    md5 = MD5();
    return S_INVALID_URL;
  }

  String url = "";

  if (_protocol == HTTP) {
    url += "http://";
  } else {
    md5 = MD5();
    return S_INVALID_URL;
  }

  url += _site;
  if (_port == "") {
    md5 = MD5();
    return S_INVALID_URL;
  } else if (_port != "80") {
    url += ":" + _port;
  }
  
  if (_file != "") {
    url += "/" + _file;
  }

  MD5Caculator md5caculator(url);
  md5caculator.digest(_md5);
  md5 = _md5;
  has_get_md5 = 1;

  return S_OK;
}

RES_CODE Url::get_retries(int& retries) {
  retries = _retries;

  return S_OK;
}

RES_CODE Url::inc_retries() {
  ++_retries;

  return S_OK;
}

_END_MYJFM_NAMESPACE_

