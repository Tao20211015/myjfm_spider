/*******************************************************************************
 * dns_cache.cc - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _DNS_CACHE_H_
#define _DNS_CACHE_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

class DnsCache : public Shared {
public:
  DnsCache();
  ~DnsCache();

  RES_CODE find(String&, Vector<String>&);
  RES_CODE insert(String&, Vector<String>&);
  RES_CODE update(String&, Vector<String>&);
  static RES_CODE dns_query(String&, Vector<String>&, IPTYPE&);

private:
  Map<String, Vector<String> > _dns;
};

_END_MYJFM_NAMESPACE_

#endif

