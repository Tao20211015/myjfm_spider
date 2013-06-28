#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#include "site.h"

_START_MYJFM_NAMESPACE_

Site::Site(const String& site_name) : 
  _has_get_ips(0), 
  _site_name(site_name), 
  _ip_type(IP_DUMMY) {
  _ips.clear();
}

RES_CODE Site::get_site_name(String& site_name) {
  site_name = _site_name;

  return S_OK;
}

RES_CODE Site::get_all_ips(Vector<String>& ips) {
  ips.clear();

  if (!_has_get_ips) {
    return S_NO_IP;
  }

  ips = _ips;

  return S_OK;
}

RES_CODE Site::get_one_ip(String& ip) {
  if (!_has_get_ips || _ips.size() <= 0) {
    ip = "";
    return S_NO_IP;
  }

  srand((unsigned)time(NULL));
  ip = _ips[rand() % _ips.size()];

  return S_OK;
}

RES_CODE Site::get_first_ip(String& ip) {
  if (!_has_get_ips || _ips.size() <= 0) {
    ip = "";
    return S_NO_IP;
  } else {
    ip = _ips[0];
    return S_OK;
  }
}

RES_CODE Site::get_ip_type(IPTYPE& ip_type) {
  if (!_has_get_ips || _ip_type == IP_DUMMY) {
    ip_type = IP_DUMMY;
    return S_BAD_IP_TYPE;
  }

  ip_type = _ip_type;

  return S_OK;
}

RES_CODE Site::set_ips(Vector<String>& ips) {
  if (_has_get_ips) {
    return S_HAS_SET;
  }
  
  if (ips.size() <= 0) { 
    return S_NO_INPUT;
  }

  _ips.clear();

  int i;
  for (i = 0; i < ips.size(); ++i) {
    _ips.push_back(ips[i]);
  }

  return S_OK;
}

RES_CODE Site::dns() {
  if (_has_get_ips) {
    return S_HAS_SET;
  }

  char buffer[20];

  _ips.clear();

  struct hostent* entry = gethostbyname(_site_name.c_str());

  if (!entry) {
    Cerr << "[ERROR] Can't get the IPs of the site: " << _site_name << Endl;
    return S_FAIL;
  }

  if (entry->h_addrtype == AF_INET && entry->h_length == 4) {
    while (*(entry->h_addr_list)) {
      inet_ntop(AF_INET, *(entry->h_addr_list), buffer, sizeof(buffer));
      entry->h_addr_list += 1;
      _ips.push_back(CHARS2STR(buffer));
    }
  } else if (entry->h_addrtype = AF_INET6 && entry->h_length == 6) {
    while (*(entry->h_addr_list)) {
      inet_ntop(AF_INET6, *(entry->h_addr_list), buffer, sizeof(buffer));
      entry->h_addr_list += 1;
    }
  } else {
    Cerr << "[ERROR] Invalid address!" << Endl;
    return S_UNKNOWN_PROTOCOL;
  }

  if (entry->h_addrtype == AF_INET) {
    _ip_type = IPv4;
  } else {
    _ip_type = IPv6;
  }

  _has_get_ips = 1;

  return S_OK;
}

Site::~Site() {
  _site_name = "";
  _has_get_ips = 0;
  _ips.clear();
  _ip_type = IP_DUMMY;
}

_END_MYJFM_NAMESPACE_

