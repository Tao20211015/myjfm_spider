#include <stdlib.h>
#include <time.h>

#include "site.h"

_START_MYJFM_NAMESPACE_

Site::Site(const String& site_name) {
  _site_name = site_name;
  _ips.clear();
}

void Site::get_site_name(String& site_name) {
  site_name = _site_name;
}

void Site::get_all_ips(Vector<String>& ips) {
  ips.clear();
  ips = _ips;
}

void Site::get_one_ip(String& ip) {
  srand((unsigned)time(NULL));
  if (_ips.size() <= 0) {
    ip = "";
  } else {
    ip = _ips[rand() % _ips.size()];
  }
}

void Site::get_first_ip(String& ip) {
  if (_ips.size() <= 0) {
    ip = "";
  } else {
    ip = _ips[0];
  }
}

void Site::insert_ip(const String& ip) {
  if (ip == "") {
    return;
  }
  _ips.push_back(ip);
}

void Site::insert_ips(const Vector<String>& ips) {
  int i;
  for (i = 0; i < ips.size(); ++i) {
    if (ips[i] == "") {
      continue;
    }
    _ips.push_back(ips[i]);
  }
}

Site::~Site() {
  _ips.clear();
}

_END_MYJFM_NAMESPACE_

