#include "config.h"
#include "url.h"
#include "site.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

Url::Url(const String& url) : 
  _protocol(PRO_DUMMY), 
  _site(Sharedpointer<Site>(NULL)), 
  _file(""), 
  _args(""), 
  has_get_md5(0) {
  if (url == "") {
    return;
  }

  String_size_t protocol_pos = url.find("://");
  if (protocol_pos == String_tail) {
    return;
  }

  if (url.compare(0, protocol_pos, "http") == 0) {
    _protocol = HTTP;
  } else if (url.compare(0, protocol_pos, "ftp") == 0) {
    _protocol = FTP;
  } else {
    return;
  }

  String site = "";
  String_size_t site_pos = url.find("/", protocol_pos);
  if (protocol_pos == String_tail) {
    _file = "";
    _args = "";
    site = url.substr(protocol_pos + 3);
    _site = Sharedpointer<Site>(new Site(site));
  } else {
    site = url.substr(protocol_pos + 3, site_pos - protocol_pos - 3);
    _site = Sharedpointer<Site>(new Site(site));
    String_size_t file_pos = url.find("?", site_pos);
    if (file_pos == String_tail) {
      _file = url.substr(site_pos);
      _args = "";
    } else {
      _file = url.substr(site_pos, file_pos - site_pos);
      _args = url.substr(file_pos);
    }
  }

  return;
}

RES_CODE Url::get_protocol(Protocol& protocol) {
  protocol = _protocol;

  return S_OK;
}

/*
void Url::set_protocol(Protocol protocol) {
  _protocol = protocol;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_site(Sharedpointer<Site>& site) {
  site = _site;

  return S_OK;
}

/*
void Url::set_site(Sharedpointer<Site>& site) {
  _site = site;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_file(String& file) {
  file = _file;

  return S_OK;
}

/*
void Url::set_file(String& file) {
  _file = file;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_args(String& args) {
  args = _args;

  return S_OK;
}

/*
void Url::set_args(String& args) {
  _args = args;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_md5(MD5& md5) {
  if (has_get_md5) {
    md5 = _md5;
    return S_OK;
  }

  if (_protocol == PRO_DUMMY) {
    md5 = MD5();
    return S_INVALID_URL;
  }

  String site_name;
  _site->get_site_name(site_name);

  String file;
  get_file(file);

  String args;
  get_args(args);

  String url;

  if (_protocol == HTTP) {
    url += "http://";
  } else {
    url += "ftp://";
  }

  url += site_name + file + args;

  MD5caculator md5caculator(url);
  md5caculator.digest(_md5);
  md5 = _md5;
  has_get_md5 = 1;

  return S_OK;
}

_END_MYJFM_NAMESPACE_

