#include "config.h"
#include "url.h"
//#include "site.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

Url::Url(const String& url) : 
  _raw_url(""), 
  _is_valid(0), 
  _protocol(PRO_DUMMY), 
  _site(""), 
  //_site(SharedPointer<Site>(NULL)), 
  _port(""), 
  _file(""), 
  _args(""), 
  has_get_md5(0) {
  if (url == "") {
    return;
  }

  /*
  String_size_t protocol_pos = url.find("://");
  if (protocol_pos == String_tail) {
    return;
  }

  if (url.compare(0, protocol_pos, "http") == 0) {
    _protocol = HTTP;
  //} else if (url.compare(0, protocol_pos, "ftp") == 0) {
  //  _protocol = FTP;
  } else {
    return;
  }

  String site = "";
  String_size_t site_pos = url.find("/", protocol_pos + 3);
  if (protocol_pos == String_tail) {
    _file = "";
    _args = "";
    _site = url.substr(protocol_pos + 3);
    String_size_t port_pos = _url.find(":", protocol_pos + 3);
    //site = url.substr(protocol_pos + 3);
    //_site = SharedPointer<Site>(new Site(site));
  } else {
    _site = url.substr(protocol_pos + 3, site_pos - protocol_pos - 3);
    //site = url.substr(protocol_pos + 3, site_pos - protocol_pos - 3);
    //_site = SharedPointer<Site>(new Site(site));
    String_size_t file_pos = url.find("?", site_pos);
    if (file_pos == String_tail) {
      _file = url.substr(site_pos + 1);
      _args = "";
    } else {
      _file = url.substr(site_pos + 1, file_pos - site_pos - 1);
      _args = url.substr(file_pos + 1);
    }
  }
  */

  String_size_t site_pos = -1;
  String_size_t port_pos = -1;
  String_size_t file_pos = -1;

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
  if (site_pos == String_tail) { // no requested file and args
    port_pos = no_proto_url.find(":");
    if (port_pos == String_tail) { // not specify port
      _site = no_proto_url;
      _port = "80"; // default port is 80
    } else {
      _site = no_proto_url.substr(0, port_pos);
      _port = no_proto_url.substr(port_pos + 1);
    }
  } else { // there are file and args parameters in the url
    port_pos = no_proto_url.find(":");
    if (port_pos == String_tail || port_pos > site_pos) { // not specify port
      _site = no_proto_url.substr(0, site_pos);
      _port = "80";
    } else {
      _site = no_proto_url.substr(0, port_pos);
      _port = no_proto_url.substr(port_pos + 1, site_pos - port_pos - 1);
    }

    String no_site_url = no_proto_url.substr(site_pos + 1);
    file_pos = no_site_url.find("?");
    if (file_pos == String_tail) { // no args
      _file = no_site_url;
    } else {
      _file = no_site_url.substr(0, file_pos);
      _args = no_site_url.substr(file_pos + 1);
    }
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

/*
void Url::set_protocol(Protocol protocol) {
  _protocol = protocol;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_site(String& site) {
  site = _site;

  return S_OK;
}

/*
RES_CODE Url::get_site(SharedPointer<Site>& site) {
  site = _site;

  return S_OK;
}
*/

/*
void Url::set_site(SharedPointer<Site>& site) {
  _site = site;
  has_get_md5 = 0;
}
*/

RES_CODE Url::get_port(String& port) {
  port = _port;

  return S_OK;
}

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

  if (!_is_valid || _protocol == PRO_DUMMY) {
    md5 = MD5();
    return S_INVALID_URL;
  }

  String url = "";

  if (_protocol == HTTP) {
    url += "http://";
  } else {
  //  url += "ftp://";
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

  if (_args != "") {
    url += "?" + _args;
  }

  std::cout << "!!!" << url << "!!!" << std::endl;
  MD5Caculator md5caculator(url);
  md5caculator.digest(_md5);
  md5 = _md5;
  has_get_md5 = 1;

  return S_OK;
}

_END_MYJFM_NAMESPACE_

