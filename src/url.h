/*******************************************************************************
 * url.h - the url class
 * it includes the protocol(HTTP), the site's name, the port, the requested file
 * name, etc.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _URL_H_
#define _URL_H_

#include <stdint.h>

#include "config.h"
#include "shared.h"
#include "md5.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

class Url : public Shared {
public:
  enum EnumStatus {
    UNINITIALIZED = 0x0, 
    INITIALIZED = 0x1, 
    DNSED = 0x2
  };

  Url(const String& url = "");
  ~Url() {}

  RES_CODE get_status(EnumStatus&);

  RES_CODE get_protocol(Protocol&);

  RES_CODE set_ip(Vector<uint32_t>&);

  RES_CODE get_ip(Vector<uint32_t>&);

  RES_CODE get_port(uint16_t&);

  RES_CODE get_site(String&);

  RES_CODE get_file(String&);

  RES_CODE get_md5(MD5&);

  RES_CODE get_retries(uint32_t&);

  RES_CODE inc_retries();
  
  bool get_has_dnsed_cname();

  RES_CODE set_has_dnsed_cname(bool has_dnsed_cname);

private:
  String _raw_url;
  volatile EnumStatus _status;
  Protocol _protocol;
  Vector<uint32_t> _ips;
  uint16_t _port;
  String _site;
  String _file;

  bool has_get_md5;
  MD5 _md5;

  uint32_t _retries;
  bool _has_dnsed_cname;
};

_END_MYJFM_NAMESPACE_

#endif

