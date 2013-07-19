/*******************************************************************************
 * dns_cache.cc - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "config.h"
#include "dns_cache.h"
#include "log.h"
#include "rwlock.h"

_START_MYJFM_NAMESPACE_

DnsCache::DnsCache() {
  _dns.clear();
}

DnsCache::~DnsCache() {
  _dns.clear();
}

RES_CODE DnsCache::find(String& site, Vector<uint32_t>& ips) {
  if (site.length() == 0 || ips.size() > 0) {
    return S_FAIL;
  }

  RWlock::ReadScopeGuard guard(&_rwlock);

  Map<String, Vector<uint32_t> >::iterator itr = _dns.find(site);

  if (itr == _dns.end()) {
    return S_NOT_FOUND;
  }

  ips = itr->second;

  return S_OK;
}

RES_CODE DnsCache::insert(String& site, Vector<uint32_t>& ips) {
  if (site.length() == 0 || ips.size() == 0) {
    return S_FAIL;
  }

  RWlock::WriteScopeGuard guard(&_rwlock);
  _dns[site] = ips;
  return S_OK;
}

_END_MYJFM_NAMESPACE_

