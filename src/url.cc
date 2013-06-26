#include "config.h"
#include "url.h"
#include "site.h"


_START_MYJFM_NAMESPACE_

Url::Url(const String& url) : 
  _protocol(DUMMY), 
  _site(Site()), 
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
    _site = Site(site);
  } else {
    site = url.substr(protocol_pos + 3, site_pos - protocol_pos - 3);
    _site = Site(site);
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

Protocol Url::get_protocol() {
  return _protocol;
}

/*
void Url::set_protocol(Protocol protocol) {
  _protocol = protocol;
  has_get_md5 = 0;
}
*/

void Url::get_site(Site& site) {
  site = _site;
}

/*
void Url::set_site(Site& site) {
  _site = site;
  has_get_md5 = 0;
}
*/

void Url::get_file(String& file) {
  file = _file;
}

/*
void Url::set_file(String& file) {
  _file = file;
  has_get_md5 = 0;
}
*/

void Url::get_args(String& args) {
  args = _args;
}

/*
void Url::set_args(String& args) {
  _args = args;
  has_get_md5 = 0;
}
*/

int Url::get_md5(MD5& md5) {
  if (has_get_md5) {
    md5 = _md5;
    return 0;
  }

  if (_protocol == DUMMY) {
    md5 = MD5();
    return -1;
  }

  String site_name;
  _site.get_site_name(site_name);

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
}

_END_MYJFM_NAMESPACE_

