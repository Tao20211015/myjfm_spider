#ifndef _SITE_H_
#define _SITE_H_

#include "config.h"
#include "global.h"

_START_MYJFM_NAMESPACE_

class Site {
public:
  Site(const String& site_name = "");
  ~Site();
  void get_site_name(String& site_name);
  void get_all_ips(Vector<String>& ips);
  void get_one_ip(String& ip);
  void get_first_ip(String& ip);
  void insert_ip(const String& ip);
  void insert_ips(const Vector<String>& ips);

private:
  String _site_name;
  Vector<String> _ips;
};

_END_MYJFM_NAMESPACE_

#endif

