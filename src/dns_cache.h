/*******************************************************************************
 * dns_cache.h - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _DNS_CACHE_H_
#define _DNS_CACHE_H_

#include <stdint.h>

#include "config.h"
#include "shared.h"
#include "rwlock.h"

_START_MYJFM_NAMESPACE_

class DnsCache : public Shared {
public:
  DnsCache();
  ~DnsCache();

  RES_CODE find(String&, Vector<uint32_t>&);
  RES_CODE insert(String&, Vector<uint32_t>&);
  static RES_CODE dns_query(String&, Vector<uint32_t>&, IPTYPE&);

private:
  Map<String, Vector<uint32_t> > _dns;
  RWlock _rwlock;
};

_END_MYJFM_NAMESPACE_

#endif

