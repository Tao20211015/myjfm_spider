/*******************************************************************************
 * site.h - the class includes the web site's name, ips
 * this class is abandoned
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SITE_H_
#define _SITE_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

class Site : public Shared {
public:
  Site(const String& site_name = "");
  ~Site();

  RES_CODE get_site_name(String&);
  RES_CODE get_all_ips(Vector<String>&);
  RES_CODE get_one_ip(String&);
  RES_CODE get_first_ip(String&);
  RES_CODE get_ip_type(IPTYPE&);

  // get ip addresses from DNS cache
  RES_CODE set_ips(Vector<String>& ips);

  // get ip addresses from DNS query
  // although we provide this routine, you'd better not use it
  RES_CODE dns();

private:
  int _has_get_ips;
  String _site_name;
  Vector<String> _ips;
  IPTYPE _ip_type;
};

_END_MYJFM_NAMESPACE_

#endif

