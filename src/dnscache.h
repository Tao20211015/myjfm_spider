#ifndef _DNSCACHE_H_
#define _DNSCACHE_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

class Dnscache : public Shared {
public:
  Dnscache();
  ~Dnscache();

  RES_CODE find(String&, Vector<String>&);
  RES_CODE insert(String&, Vector<String>&);
  RES_CODE update(String&, Vector<String>&);
  static RES_CODE dns_query(String&, Vector<String>&, IPTYPE&);

private:
  Map<String, Vector<String> > _dns;
};

_END_MYJFM_NAMESPACE_

#endif

